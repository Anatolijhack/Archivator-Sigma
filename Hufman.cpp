
#include "Hufman.h"


// ================= BUFFER =================

BufferReader::BufferReader(const std::vector<char>& d) : data(d) {}

bool BufferReader::readByte(uint8_t& b)
{
    if (pos >= data.size()) return false;
    b = (uint8_t)data[pos++];
    return true;
}

bool BufferReader::readData(void* dst, size_t size)
{
    if (pos + size > data.size()) return false;
    memcpy(dst, data.data() + pos, size);
    pos += size;
    return true;
}

void BufferWriter::writeByte(uint8_t b)
{
    buffer.push_back((char)b);
}

void BufferWriter::writeData(const void* data, size_t size)
{
    size_t old = buffer.size();
    buffer.resize(old + size);
    memcpy(buffer.data() + old, data, size);
}

// ================= BIT =================

BitWriter::BitWriter(BufferWriter& out) : out(out) {}

void BitWriter::writeBit(bool bit)
{
    byte = (byte << 1) | (bit ? 1 : 0);
    count++;

    if (count == 8)
    {
        out.writeByte(byte);
        byte = 0;
        count = 0;
    }
}

void BitWriter::flush()
{
    if (count > 0)
    {
        byte <<= (8 - count);
        out.writeByte(byte);
        byte = 0;
        count = 0;
    }
}

BitReader::BitReader(BufferReader& in) : in(in) {}

bool BitReader::readBit(bool& bit)
{
    if (count == 0)
    {
        if (!in.readByte(byte))
            return false;
        count = 8;
    }

    bit = (byte & 0x80) != 0;
    byte <<= 1;
    count--;

    return true;
}

// ================= NODE =================

Node::Node(uint8_t value, uint64_t fcount)
{
    left = nullptr;
    right = nullptr;
    this->value = value;
    this->fcount = fcount;
}

Node::Node(Node* left, Node* right)
{
    this->left = left;
    this->right = right;

    uint64_t lf = left ? left->fcount : 0;
    uint64_t rf = right ? right->fcount : 0;

    this->fcount = lf + rf;
}

bool Comprase::operator()(Node* left, Node* right)
{
    return left->fcount > right->fcount;
}

// ================= INTERNAL =================

static Node* BuildTree(const std::unordered_map<uint8_t, uint64_t>& freq)
{
    std::priority_queue<Node*, std::vector<Node*>, Comprase> pq;

    for (auto& [c, f] : freq)
        pq.push(new Node(c, f));

    if (pq.empty()) return nullptr;
    if (pq.size() == 1) return pq.top();

    while (pq.size() > 1)
    {
        Node* a = pq.top(); pq.pop();
        Node* b = pq.top(); pq.pop();
        pq.push(new Node(a, b));
    }

    return pq.top();
}

static void DeleteTree(Node* node)
{
    if (!node) return;
    DeleteTree(node->left);
    DeleteTree(node->right);
    delete node;
}

static void buildCodes(Node* node, std::string& path,
    std::unordered_map<uint8_t, std::string>& codes)
{
    if (!node) return;

    if (!node->left && !node->right)
    {
        if (path.empty()) path = "0";
        codes[node->value] = path;
        return;
    }

    path.push_back('0');
    buildCodes(node->left, path, codes);
    path.pop_back();

    path.push_back('1');
    buildCodes(node->right, path, codes);
    path.pop_back();
}

// ================= TREE =================

void HuffmanCodec::WriteTree(Node* node, BufferWriter& out)
{
    if (!node->left && !node->right)
    {
        out.writeByte(1);
        out.writeByte(node->value);
        return;
    }

    out.writeByte(0);
    WriteTree(node->left, out);
    WriteTree(node->right, out);
}

Node* HuffmanCodec::ReadTree(BufferReader& in)
{
    uint8_t flag;
    if (!in.readByte(flag)) return nullptr;

    if (flag == 1)
    {
        uint8_t value;
        in.readByte(value);
        return new Node(value, 0);
    }

    Node* left = ReadTree(in);
    Node* right = ReadTree(in);

    return new Node(left, right);
}

// ================= COMPRESS =================
//
//std::vector<char> HuffmanCodec::compress(const std::vector<char>& input)
//{
//    BufferWriter out;
//
//    std::unordered_map<uint8_t, uint64_t> freq;
//    for (uint8_t c : input)
//        freq[c]++;
//
//    if (freq.empty()) return {};
//
//    Node* root = BuildTree(freq);
//
//    std::unordered_map<uint8_t, std::string> codes;
//    std::string path;
//    buildCodes(root, path, codes);
//
//    WriteTree(root, out);
//
//    uint64_t size = input.size();
//    out.writeData(&size, sizeof(size));
//
//    BitWriter bw(out);
//
//    for (uint8_t c : input)
//        for (char b : codes[c])
//            bw.writeBit(b == '1');
//
//    bw.flush();
//
//    DeleteTree(root);
//    return out.buffer;
//}

// ================= DECOMPRESS =================

//std::vector<char> HuffmanCodec::decompress(const std::vector<char>& data)
//{
//    BufferReader in(data);
//
//    Node* root = ReadTree(in);
//
//    uint64_t size = 0;
//    if (!in.readData(&size, sizeof(size)))
//        throw std::runtime_error("Broken Huffman header");
//
//    std::vector<char> out;
//    out.reserve(size);
//
//    if (!root) return out;
//
//    if (!root->left && !root->right)
//    {
//        out.assign(size, root->value);
//        DeleteTree(root);
//        return out;
//    }
//
//    BitReader br(in);
//
//    Node* cur = root;
//    bool bit;
//
//    while (out.size() < size)
//    {
//        if (!br.readBit(bit))
//            throw std::runtime_error("Unexpected end");
//
//        cur = bit ? cur->right : cur->left;
//
//        if (!cur)
//            throw std::runtime_error("Corrupted tree");
//
//        if (!cur->left && !cur->right)
//        {
//            out.push_back(cur->value);
//            cur = root;
//        }
//    }
//
//    DeleteTree(root);
//    return out;
//}
void HuffmanCodec::compressStream(
    BufferReader& in,
    BufferWriter& out)
{
    // 1. читаем всё (пока chunk-based, это ок)
    std::vector<uint8_t> data;

    uint8_t b;
    while (in.readByte(b))
        data.push_back(b);

    if (data.empty())
        return;

    // 2. частоты
    std::unordered_map<uint8_t, uint64_t> freq;
    for (auto c : data)
        freq[c]++;

    Node* root = BuildTree(freq);

    // 3. коды
    std::unordered_map<uint8_t, std::string> codes;
    std::string path;
    buildCodes(root, path, codes);

    // 4. дерево
    WriteTree(root, out);

    // 5. размер оригинала
    uint64_t size = data.size();
    out.writeData(&size, sizeof(size));

    // 6. потоковая запись битов
    BitWriter bw(out);

    for (auto c : data)
    {
        const std::string& code = codes[c];
        for (char bit : code)
            bw.writeBit(bit == '1');
    }

    bw.flush();

    DeleteTree(root);
}
void HuffmanCodec::decompressStream(
    BufferReader& in,
    BufferWriter& out)
{
    Node* root = ReadTree(in);

    uint64_t size = 0;
    if (!in.readData(&size, sizeof(size)))
        throw std::runtime_error("Broken Huffman header");

    if (!root)
        return;

    // special case (1 символ)
    if (!root->left && !root->right)
    {
        for (uint64_t i = 0; i < size; i++)
            out.writeByte(root->value);

        DeleteTree(root);
        return;
    }

    BitReader br(in);

    Node* cur = root;
    bool bit;

    uint64_t written = 0;

    while (written < size)
    {
        if (!br.readBit(bit))
            throw std::runtime_error("Unexpected end");

        cur = bit ? cur->right : cur->left;

        if (!cur)
            throw std::runtime_error("Corrupted tree");

        if (!cur->left && !cur->right)
        {
            out.writeByte(cur->value);
            cur = root;
            written++;
        }
    }

    DeleteTree(root);
}
std::vector<char> HuffmanCodec::compress(const std::vector<char>& input)
{
    BufferReader in(input);
    BufferWriter out;

    compressStream(in, out);

    return out.buffer;
}