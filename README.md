## ~~~spaghetti test~~~
 
### Để chạy đuợc bài test thì hãy tự thêm VectorStore.cpp và .h vào repo
### sau đó thêm  ```template std::string ArrayList<VectorStore::VectorRecord \*>::toString( std::string (*)(VectorStore::VectorRecord *&)) const;```
### ở cuối file VectorStore.cpp 

## cách chạy bài test

### Đối với nguời không dùng CMake(beta) 
### dùng g++ -std=c++17 main.cpp VectorStore.cpp -I. -o main

### Đối với nguời dùng CMake(based)
### cmake -S. -B _build_dir_  -G _build_tool_của_bạn_

### lưu ý nếu _build_dir_ không phải thư mục root 
### ví dụ như cmake -S. -Bbuild -G make
### thì hãy copy thư mục test vào trong thư mục build của bạn 
### hãy đọc file CMakeLists.txt nếu muốn bật thêm flag 
### tác giả đã cài sẵn definition run nên chỉ cần chạy _build_tool_của_bạn_ + run ở trong thư mục build
### ví dụ : make run, ninja run, etc

### tác giả không chịu trách nhiệm nếu nguời dùng không đọc được code của test 
### có ý kiến gì hãy mở pr
