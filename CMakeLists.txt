cmake_minimum_required(VERSION 3.10)
project(MathFormula)

# 添加源文件到动态库
add_library(mathformula SHARED src/mathformula.c src/pstring.c)

# 添加头文件目录
target_include_directories(mathformula PUBLIC include)

# 添加静态库
add_library(mathformula_static STATIC src/mathformula.c src/pstring.c)

# 添加头文件目录
target_include_directories(mathformula_static PUBLIC include)

# 添加可执行文件
add_executable(test examples/test.c)
add_executable(test2 examples/test2.c)

# 链接动态库
target_link_libraries(test mathformula m)

# 链接静态库
target_link_libraries(test2 mathformula_static m)
