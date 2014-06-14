#ifndef GET_FILEBUF_H
#define GET_FILEBUF_H

#include <stdio.h>
#ifdef __MINGW32__
    #include <ext/stdio_filebuf.h>
    #include <share.h>
#endif

class char_filebuf : public __gnu_cxx::stdio_filebuf<char, std::char_traits<char>> {
    public:
    char_filebuf() : stdio_filebuf() {}
    char_filebuf(FILE* file, std::ios::openmode mode, const wchar_t* fn) : stdio_filebuf(file, mode), filename(fn) {}
    virtual ~char_filebuf() {
        std::wcout << L"Closing file :" << filename << std::endl;
        fclose(this->_M_file.file());
    }
    const wchar_t* filename;
};

class uifstream : public std::basic_istream<char, std::char_traits<char>> {
    public:
    uifstream(const boost::filesystem::path& path, std::ios::openmode mode = std::ios::in) {
        std::wstring modestr = L"r";
        if (mode & std::ios::binary)
            modestr += L"b";
        FILE* file = _wfsopen(path.wstring().c_str(), modestr.c_str(), _SH_DENYNO);
        rdbuf(new char_filebuf(file, mode, path.wstring().c_str()));
    }
    virtual ~uifstream() {
        delete rdbuf();
    }
};

class uofstream : public std::basic_ostream<char, std::char_traits<char>> {
    public:
    uofstream(const boost::filesystem::path& path, std::ios::openmode mode = std::ios::out) {
        std::wstring modestr = L"w";
        if (mode & std::ios::app) {
            modestr = L"a";
            // Append mode doesn't seem to work with stdio_filebuf at all.
            mode &= ~std::ios::app;
            mode |= std::ios::out;
        }
        if (mode & std::ios::binary)
            modestr += L"b";
        FILE* file = _wfsopen(path.wstring().c_str(), modestr.c_str(), _SH_DENYNO);
        rdbuf(new char_filebuf(file, mode, path.wstring().c_str()));
    }
    virtual ~uofstream() {
        delete rdbuf();
    }
};

inline char_filebuf* get_ifilebuf(const boost::filesystem::path& path, std::ios::openmode mode = std::ios::in) {
    std::wstring modestr = L"r";
    if (mode & std::ios::binary)
        modestr += L"b";
    FILE* fin = _wfsopen(path.wstring().c_str(), modestr.c_str(), _SH_DENYNO);
    return new char_filebuf(fin, mode, path.wstring().c_str());
}

inline char_filebuf* get_ofilebuf(const boost::filesystem::path& path, std::ios::openmode mode = std::ios::out) {
    std::wstring modestr = L"w";
    if (mode & std::ios::app) {
        modestr = L"a";
        // Append mode doesn't seem to work at all.
        mode &= ~std::ios::app;
        mode |= std::ios::out;
    }
    if (mode & std::ios::binary)
        modestr += L"b";
    FILE* fout = _wfsopen(path.wstring().c_str(), modestr.c_str(), _SH_DENYNO);
    return new char_filebuf(fout, mode, path.wstring().c_str());
}

#endif // GET_FILEBUF_H
