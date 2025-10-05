# ~~~Spaghetti Test~~~

### Setup

1. Thêm `VectorStore.cpp` và `VectorStore.h` vào repo.
2. Thêm dòng sau ở **cuối file `VectorStore.cpp`**:

```cpp
template std::string ArrayList<VectorStore::VectorRecord*>::toString(
    std::string (*)(VectorStore::VectorRecord *&)
) const;
```

---

### Cách chạy bài test

#### 1. Với người không dùng CMake (beta)
```bash
g++ -std=c++17 main.cpp VectorStore.cpp -I. -o main
./main
```

#### 2. Với người dùng CMake (gigachad)
```bash
cmake -S . -B <build_dir> -G <build_tool_của_bạn>
cmake --build <build_dir> --target run
```

**Lưu ý:**
- Nếu `<build_dir>` không phải thư mục root, copy thư mục `test` vào trong thư mục build.  
- Đọc file `CMakeLists.txt` nếu muốn bật thêm flag.  
- `run` target đã được cài sẵn; chỉ cần chạy build tool trong thư mục build, ví dụ:

```bash
make run      # nếu dùng Make
ninja run     # nếu dùng Ninja
```

---

### Lưu ý quan trọng

- Tác giả **không chịu trách nhiệm** nếu người dùng không chạy được test.  
- Có ý kiến gì hãy mở **PR**.
