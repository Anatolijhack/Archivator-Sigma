#pragma once
class IDataSource {
public:
    virtual ~IDataSource() = default;

    
    virtual size_t read(char* buffer, size_t maxSize) = 0;

    
    virtual void seek(size_t pos) = 0;
};
