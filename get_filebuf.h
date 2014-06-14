#ifndef GET_FILEBUF_H
#define GET_FILEBUF_H

#include <stdio.h>
#ifdef __MINGW32__
    #include <ext/stdio_filebuf.h>
    #include <share.h>
#endif

typedef std::basic_filebuf<char, std::char_traits<char>> char_filebuf;

inline char_filebuf* get_ifilebuf(const boost::filesystem::path& path, std::ios::openmode mode = std::ios::in) {
    std::wstring modestr = L"r";
    if (mode & std::ios::binary)
        modestr += L"b";
    FILE* fin = _wfsopen(path.wstring().c_str(), modestr.c_str(), _SH_DENYNO);
    int fd = fileno(fin); // fin gets leaked
    return new __gnu_cxx::stdio_filebuf<char, std::char_traits<char>>(fd, mode);
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
    int fd = fileno(fout); // fout gets leaked
    return new __gnu_cxx::stdio_filebuf<char, std::char_traits<char>>(fd, mode);
}

#endif // GET_FILEBUF_H
