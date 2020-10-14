# CMake 튜토리얼 Lv.2

- [CMake의 목적과 기능범위](#cmake의-목적과-기능범위)
  - [CMake에서 Build로 이어지는 과정](#cmake에서-build로-이어지는-과정)
- [Command](#command)
  - [파일 생성하기](#파일-생성)
- [Target](#target)
  - [추가 설명](#추가-설명)
  - [Pseudo Target](#pseudo-target)
  - [Custom Target](#custom-target)

## CMake의 목적과 기능범위

빌드 시스템(build system)이 하는 일은 연속적인 이벤트(컴파일러/링커 호출)를 통해 최종적으로는 프로그램을 생성하는 것입니다. 빌드 시스템 파일, 혹은 프로젝트 파일은 프로그래머의 의도에 맞게 컴파일러/링커 호출을 조직화한 명령서라고 할 수 있습니다. 

1편에서 강조한 대로라면 CMake는 '명령서'를 작성하는 것으로 그 기능이 충분하겠지만, 실제 기능적으로는 좀 더 넓은 지원범위를 가지고 있습니다. CMake가 중심이 되는 프로젝트라면, 이를 활용해 빌드에 필요한 소스 파일까지 생성할 수 있습니다.

>
> 작성자는 이 기능들을 사용하는 것을 추천하지 않습니다.  
> 빌드 시스템 파일을 생성하는것이 CMake의 가장 중요한 부분이며, 오직 그 일에 집중해야 한다고 생각하기 때문입니다
>

* CMake의 목적: 프로젝트(빌드 시스템) 파일 생성
* CMake의 실제 지원범위
  * 프로젝트 생성 및 속성, 의존성 설정
     * `add_library`, `add_executable`, `set_target_properties`, `add_dependencies`...
  * 프로그램 호출
     * [`add_custom_command`](https://cmake.org/cmake/help/latest/command/add_custom_command.html#generating-files)
     * [`add_custom_target`](https://cmake.org/cmake/help/latest/command/add_custom_target.html)

### [CMake에서 Build로 이어지는 과정](https://cgold.readthedocs.io/en/latest/tutorials/cmake-stages.html)

CMake를 사용해 빌드 시스템 파일을 생성하는 과정은 2 pass assembler와 유사하다고 할 수 있습니다. 

* Configuration
  * `CMakeLists.txt` 문법 검사
  * Macro, Function 실행
  * Cmake Cache 생성
* Generation
  * **Target**에서 Build System Project 생성

Configuration 단계에서는 CMakeList를 해석하고, 재사용 가능한 값을 토대로 `CMakeCache.txt`를 생성합니다. CMakeFiles 폴더 역시 이 단계에서 생성됩니다. 이 폴더 안에는 CMake의 log, 변경을 확인하는 stamp 파일들이 보관됩니다.

Generation은 Configuration의 정보를 바탕으로 Build System에서 사용하는 Project 파일을 생성합니다. `TargetDirectories.txt`와 같은 폴더목록도 이 단계에서 생성됩니다.

## Command

### 파일 생성

앞서 지원범위에서 CMake는 소스 파일을 생성할 수 있다고 설명하였습니다. 여기에 사용되는 것이 바로 command 입니다.

만약 존재하지 않는 파일이 CMakeList에 명시되면, CMake는 이를 오류로 처리합니다.

```console
$ tree ./wierd-project
./wierd-project
├── CMakeLists.txt
├── include
│   └── simple.h
└── src
    └── main.cpp
```

위의 프로젝트는 src폴더에 main.cpp 밖에 없습니다.  
이때 CMakeList의 내용이 아래와 같다면

```cmake
cmake_minimum_required(VERSION 3.8)

add_executable(wierd_exe
    include/simple.h

    src/main.cpp
    src/simple.cpp  # <-- ???
)

target_include_directories(wierd_exe
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
```

다음과 같은 오류를 출력할 것입니다.

```console
CMake Error at CMakeLists.txt:3 (add_executable):
  Cannot find source file:

    src/simple.cpp

  Tried extensions .c .C .c++ .cc .cpp .cxx .m .M .mm .h .hh .h++ .hm .hpp
  .hxx .in .txx

CMake Error: CMake can not determine linker language for target: wierd_exe
```

요컨대 Build System 파일을 만들기 전에 CMake에서 소스 파일이 존재하지 않는다고 경고하는 내용입니다.
이 튜토리얼에서는 스크립트 파일을 호출해서 simple.cpp를 만들어 빌드에 사용해 보겠습니다.

#### [`add_custom_command`](https://cmake.org/cmake/help/latest/command/add_custom_command.html#command:add_custom_command)


먼저 CMakeList에 새로운 내용이 추가되어야 합니다.

```cmake
cmake_minimum_required(VERSION 3.8)

# we will generate the file with the given command
add_custom_command( 
    OUTPUT      src/simple.cpp              # <-- output path
    COMMAND     echo    "my first command"  # <-- command + args
)

add_executable(wierd_exe
    include/simple.h

    src/main.cpp
    src/simple.cpp
)

target_include_directories(wierd_exe
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
```

다시 CMake를 실행해보면 이번엔 문제없이 Generation 단계까지 마치는 것을 확인할 수 있을 것입니다.

```console
PS C:\wierd-project\build> cmake .. -G "Visual Studio 15 2017 Win64"
-- Configuring done
-- Generating done
-- Build files have been written to: C:/wierd-project/build
```

하지만 echo 명령이 실제로 파일을 생성하지는 않기 때문에, 빌드를 시도하면 No such file or directory 메세지와 함께 실패할 것입니다.

`add_custom_command` 함수는 기본적으로 `OUTPUT`와 `COMMAND` 인자만 제공하면 동작하지만, 보다 정확히 의도를 반영하기 위해서는 여러가지 인자를 제공해야 합니다.
이 함수를 처음 접한다면 천천히 이후의 내용을 읽어본 후, 인자를 바꿔가며 실행해보기를 권합니다.

먼저, 소스파일을 생성하는 스크립트를 작성해서, COMMAND에서 이를 호출하도록 하여 파일이 생성되는 위치를 확인할 수 있습니다. 

```console
$ tree ./wierd-project
./wierd-project
├── CMakeLists.txt
├── include
│   └── simple.h
├── scripts
│   └── create_simple_cpp.sh # <--- src file generation script
└── src
    └── main.cpp
```

소스 파일을 생성하는 스크립트의 내용은 아주 단순합니다

```sh
# create_simple_cpp.sh
echo "extern const int version = 3;" > src/simple.cpp;
```

위 내용은 Windows의 CLI에서도 실행될 수 있기 때문에 따로 셔뱅(shebang)을 두지 않았습니다.
UNIX 환경이라면 기본 shell 환경(zsh, bash 등)을 따르면 됩니다. 예시에서는 bash로 가정하겠습니다.

```cmake
cmake_minimum_required(VERSION 3.8)

# more detailed src file generation command
if(WIN32)
    add_custom_command(
        OUTPUT      src/simple.cpp
        COMMAND     call ${CMAKE_CURRENT_SOURCE_DIR}/scripts/create_simple_cpp.sh 
        COMMENT     "creating simple.cpp"
    )
elseif(UNIX)
    add_custom_command(
        OUTPUT      src/simple.cpp
        COMMAND     bash ${CMAKE_CURRENT_SOURCE_DIR}/scripts/create_simple_cpp.sh 
        COMMENT     "creating simple.cpp"
    )
endif()

# ... add_executable ...
```

Windows에서는 Command Prompt를 사용하게 됩니다. (call 을 사용하는 것을 보고 직감하셨나요?) Powershell이 아님에 주의하시기 바랍니다.  
**이대로 CMake를 실행하면 Comment가 출력되지 않는 것을 볼 수 있습니다**. 이는 Configuration/Generation 단계에서는 command가 실행되지 않는다는 의미입니다.

실제로 빌드를 실행했을때 comment가 출력된 것을 볼 수 있습니다.

```console
/path/to/wierd-project/build$ make
[ 25%] creating simple.cpp                      # <-----
Scanning dependencies of target wierd_exe
[ 50%] Building CXX object CMakeFiles/wierd_exe.dir/src/main.cpp.o
[ 75%] Building CXX object CMakeFiles/wierd_exe.dir/src/simple.cpp.o
[100%] Linking CXX executable wierd_exe
[100%] Built target wierd_exe
```

이제까지는 상대경로를 사용하면 CMakeList를 기준으로, 즉 SOURCE_DIR를 기준으로 파일을 참조하였습니다.
하지만 `add_custom_command`의 생성 파일은 빌드 폴더(BINARY_DIR)를 기준으로 합니다. 

> 만약 여기서 빌드가 실패했고, **스크립트 실행 중 no such file or directory 오류**가 발생한다면,  
> 이는 build 폴더 내에 src 폴더가 없기 때문일 것입니다. 
> (create_simple_cpp.sh 스크립트에는 폴더를 생성하는 내용이 없기 때문)  
> 그런 경우 src 폴더를 생성한 뒤 다시 시도해보시기 바랍니다.

프로젝트 폴더를 tree로 조회하면 src 폴더에는 여전히 main.cpp만 존재하는 것을 확인할 수 있습니다.

```console
/path/to/wierd-project$ tree -L 3 .
.          # <--------------- CMAKE_CURRENT_SOURCE_DIR
├── CMakeLists.txt  
├── build       # <--------------- CMAKE_CURRENT_BINARY_DIR
│   ├── CMakeCache.txt
│   ├── CMakeFiles
│   │   ├── 3.10.2
|   |   ...
│   │   └── wierd_exe.dir
│   ├── Makefile
│   ├── cmake_install.cmake
│   ├── src
│   │   └── simple.cpp  # <-------- generated file
│   └── wierd_exe
├── include
│   └── simple.h
├── scripts
│   └── create_simple_cpp.sh
└── src
    └── main.cpp    # <----- where is my friend?
```

이런 경우, 빌드 과정에서 생성하는 파일을 어떻게 관리할 것인지 먼저 생각해봐야 합니다. 이 튜토리얼에서는 다음의 2가지 방법을 보이겠습니다.

* Working directory를 지정
* Script에 인자를 제공

#### Working Directory + `add_custom_command`

CLI 환경에서 working directory는 명령을 호출한 위치를 의미합니다. Bash에서는 `pwd`, PowerShell에서는 `Get-Location` 명령으로 확인할 수도 있습니다.
바로 위에서 실행한 tree의 경우, `/path/to/wierd-project`가 working directory가 됩니다.

[`add_custom_command`는 선택 인자로 `WORKING_DIRECTORY`를 명시할 수 있습니다](https://cmake.org/cmake/help/latest/command/add_custom_command.html). 이 경로를 SOURCE_DIR로 바꿔서 실행시켜 보겠습니다.

```cmake
# ...
if(WIN32)
    add_custom_command(
        OUTPUT      src/simple.cpp
        COMMAND     call ${CMAKE_CURRENT_SOURCE_DIR}/scripts/create_simple_cpp.sh 
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT     "creating simple.cpp"
    )
elseif(UNIX)
    add_custom_command(
        OUTPUT      src/simple.cpp
        COMMAND     bash ${CMAKE_CURRENT_SOURCE_DIR}/scripts/create_simple_cpp.sh 
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT     "creating simple.cpp"
    )
endif()
# ...
```

이렇게 수정하면 아래와 같이 **오류가 발생해야 합니다**.

```console
/path/to/wierd-project/build$ make
[ 25%] creating simple.cpp
Scanning dependencies of target wierd_exe
[ 50%] Building CXX object CMakeFiles/wierd_exe.dir/src/main.cpp.o
[ 50%] creating simple.cpp
[ 75%] Building CXX object CMakeFiles/wierd_exe.dir/src/simple.cpp.o
c++: error: /path/to/wierd-project/build/src/simple.cpp: No such file or directory
c++: fatal error: no input files
compilation terminated.
CMakeFiles/wierd_exe.dir/build.make:90: recipe for target 'CMakeFiles/wierd_exe.dir/src/simple.cpp.o' failed
make[2]: *** [CMakeFiles/wierd_exe.dir/src/simple.cpp.o] Error 1
CMakeFiles/Makefile2:67: recipe for target 'CMakeFiles/wierd_exe.dir/all' failed
make[1]: *** [CMakeFiles/wierd_exe.dir/all] Error 2
Makefile:83: recipe for target 'all' failed
make: *** [all] Error 2
```

Windows 환경, Generator가 Visual Studio인 경우에도 마찬가지입니다.

```console
Build FAILED.

"C:\wierd-project\build\ALL_BUILD.vcxproj" (default target) (1) ->
"C:\wierd-project\build\wierd_exe.vcxproj" (default target) (3) ->
(ClCompile target) ->
  c1xx : fatal error C1083: Cannot open source file: 'C:\wierd-project\build\src\simple.cpp': No such file or directory [C:\wierd-project\build\wierd_exe.vcxproj]

    0 Warning(s)
    1 Error(s)

Time Elapsed 00:00:02.92
```

오류메세지의 경로를 확인해보면 두 경우 모두 build 폴더에서 src/simple.cpp를 찾으려 했다는 것을 알 수 있습니다. **이는 `add_custom_command`의 OUTPUT이 묵시적으로 BINARY_DIR를 기준으로 하기 때문입니다.** OUTPUT 인자를 절대경로로 변경하면 빌드가 성공하는 것을 확인할 수 있을 것입니다.

```cmake
cmake_minimum_required(VERSION 3.8)

if(WIN32)
    add_custom_command(
        OUTPUT      ${CMAKE_CURRENT_SOURCE_DIR}/src/simple.cpp
        COMMAND     call ${CMAKE_CURRENT_SOURCE_DIR}/scripts/create_simple_cpp.sh 
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT     "creating simple.cpp"
    )
elseif(UNIX)
    add_custom_command(
        OUTPUT      ${CMAKE_CURRENT_SOURCE_DIR}/src/simple.cpp
        COMMAND     bash ${CMAKE_CURRENT_SOURCE_DIR}/scripts/create_simple_cpp.sh 
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT     "creating simple.cpp"
    )
endif()

add_executable(wierd_exe
    include/simple.h

    src/main.cpp
    src/simple.cpp
)

target_include_directories(wierd_exe
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
```

동시에 src 폴더에 main.cpp와 simple.cpp가 함께 위치하는 것도 확인할 수 있습니다

#### Script with Arguments + `add_custom_command`

다르게 생각해보면, 코드 생성 스크립트가 너무 단순하다는 것도 문제의 원인일 수 있습니다.

스크립트 파일의 위치, 혹은 프로젝트 폴더와 같이 묵시적인 정보를 기반으로 (상대경로를 써서) 내용을 작성했지만, 
실제 스크립트는 완전히 다른 경로에서 실행되고 있을 수 있다는 점을 간과한 것이죠.
스크립트가 현재 실행되는 위치와 무관하게 동작해야 할 수도 있습니다. 
절대 경로를 인자로 제공받는다면 이 문제를 해결할 수 있을 것입니다.

```sh
# script can catch argument with $1, $2 ...
PROJECT_DIR=$1
echo "extern const int version = 3;" > $PROJECT_DIR/src/simple.cpp;
```

이제 COMMAND를 통해 스크립트에서 인자를 전달하면 됩니다. 
하지만 앞서 working directory에서 확인한 것처럼, CMake가 생성한 프로젝트는 여전히 build/src/simple.cpp를 찾을 것입니다. 따라서 OUTPUT에는 절대 경로가 필요합니다.

```cmake
# more detailed src file generation command
if(WIN32)
    add_custom_command(
        OUTPUT      ${CMAKE_CURRENT_SOURCE_DIR}/src/simple.cpp
        COMMAND     call ${CMAKE_CURRENT_SOURCE_DIR}/scripts/create_simple_cpp.sh ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT     "creating simple.cpp"
    )
elseif(UNIX)
    add_custom_command(
        OUTPUT      ${CMAKE_CURRENT_SOURCE_DIR}/src/simple.cpp
        COMMAND     bash ${CMAKE_CURRENT_SOURCE_DIR}/scripts/create_simple_cpp.sh ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT     "creating simple.cpp"
    )
endif()
```

두 방법의 차이점은 Working Directory 방법이 순수하게 CMakeList의 변경만으로 해결된 반면, Script Argument 방법은 스크립트 파일도 변경해야 했다는 부분에 있습니다.  
이는 단점이 될 수 있지만, out-of-tree build가 기본 빌드 시나리오인 경우, 혹은 다수의 스크립트가 함께 사용되는지에 따라 더 타당한 판단이 될 수 있습니다.

## Target

### 추가 설명

1편에서는 Target을 '빌드 시스템 파일 생성의 단위'라고 설명하였습니다. CMake에서 Target은 'Configuration의 대상'을 말하며, 이는 최종적으로는 Build System에서 사용하는 'Project로 Generation'됩니다. 쉽게 말해, **CMake에서의 빌드 단위**라고 요약할 수 있습니다.

초반부에 Project 파일은 '명령서'와 같다고 설명하였습니다. 보통 이 '명령'은 컴파일러/링커 호출을 의미하지만, 좀 더 일반적인 일도 포함될 수 있습니다.
일례로 Unix Makefiles 프로젝트들은 보통 install/uninstall을 지원하며, Visual Studio는 [Build event](https://docs.microsoft.com/en-us/cpp/ide/understanding-custom-build-steps-and-build-events?view=vs-2017)에 사용자 커맨드를 호출할 수 있도록 지원합니다.

Command와 Target은 의존성을 설정할 수 있고, CMake 파일의 작성자가 실행 내용(COMMAND)을 명시한다는 점이 유사합니다. 하지만 Target은 Name, Property를 가진 보다 복잡한 개체를 의미합니다.
[CMake 공식문서](https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html)에서는 Target을 다음과 같이 구분합니다.

* Binary Target
  * executable
  * library
* Pseudo Target
  * Imported Target
    * pre-existing dependency
  * Alias Target
    * read-only name

이 중 Binary target은 실제로 빌드 시스템 파일을 생성하는 경우를 의미하며, Pseudo target은 이미 존재하는 파일을 사용하는 경우만을 의미합니다. 이 문서에서는 지금까지 Binary Target 만을 중점적으로 서술했는데, 다른 타입의 Target들을 짚어보겠습니다.

### Pseudo Target

#### Imported Target

지금까지 빌드를 위해 사용해왔던 `add_executable`, `add_library` 모두 IMPORTED 옵션을 지원합니다.

```cmake
add_executable(protoc_exe IMPORTED /usr/bin/protoc)

add_library(xxx IMPORTED SHARED)
add_library(yyy IMPORTED STATIC)
```

`add_executable(IMPORTED)`는 `add_custom_command`의 COMMAND로 사용합니다. 

이미 빌드 된 라이브러리가 있는 경우 `add_library(IMPORTED)`를 사용합니다. 해당 라이브러리가 CMake가 아닌 빌드 툴을 사용해서 빌드를 수행했더라도, 별도의 CMake 파일을 작성하여 이를 CMake 프로젝트에 결합시킬 수 있습니다. 

다만 이때는 소스 파일에서부터 빌드를 수행하는 것이 아니기 때문에, `CMakeLists.txt`가 아닌 다른 cmake 설정(config) 파일을 작성하게 됩니다.  

`CMakeLists.txt`가 아닌 다른 파일을 작성한다는 것에 의아할 수 있겠지만, 라이브러리가 미리 빌드되었다는 것은 아래와 같은 정보를 전달받아야 한다는 의미이므로, **빌드와는 의미가 달라지기 때문**이라고 해석할 수 있습니다. (같은 것은 같게, 다른 것은 다르게)

* 헤더 파일을 찾을 폴더(`target_include_directory`)
* 라이브러리 의존성(`target_link_libraries`)
* 빌드할 당시에 사용한 옵션(`target_compile_options`)


이는 `find_package` 부분에서 다룰 것입니다.

#### Alias Target

이 부분은 OpenMP을 예로 들어 간단하게만 짚고 넘어가겠습니다.

```cmake
cmake_minimum_required(VERSION 3.8)

# create target without source file list
add_library(xyz IMPORTED SHARED) 

find_package(OpenMP) # https://cmake.org/cmake/help/latest/module/FindOpenMP.html
if(OpenMP_FOUND)
    set_target_properties(xyz
    PROPERTIES
        COMPILE_FLAGS "${OpenMP_CXX_FLAGS}"
    )

    if(ANDROID)
        set_target_properties(xyz
        PROPERTIES
            INTERFACE_LINK_LIBRARIES    omp     # <----- ???
        )
    else()
        set_target_properties(xyz
        PROPERTIES
            INTERFACE_LINK_LIBRARIES     OpenMP::OpenMP_CXX # <----- ???
        )
    endif() 
endif()
```

만약 이것이 특정한 빌드 시스템에서 사용하는 파일이었다면 `omp`보다는 좀 더 구체적인 이름을 사용하고 있었을 것입니다. 
플랫폼에 따라서 `omp.dll`, `omp.lib`, `libomp.a`, `libomp.so`, `libomp.dylib` 등이 될 것입니다. 

이 점을 생각하면 `omp`가 CMake Target의 이름이라는 것을 알 수 있습니다. 
CMake 파일들은 특정 플랫폼에서 사용하는 이름, 확장자에 대해 거의 신경을 쓰지 않으며, 가능한 그렇게 되도록 작성합니다. 달리 말해, CMake의 방식을 우선적으로 고려합니다.


```cmake
target_link_libraries(some_exe
PRIVATE
    xyz
)
```

별다른 주석이 없다면, CMake와 그 파일의 사용자는 위 내용에 대해서 다음과 같은 생각을 할 것입니다.

1. xyz라는 이름의 CMake Target이 존재한다
1. 만약 xyz가 Target이 아니라면, xyz는 라이브러리를 의미한다
    1. `lib[xyz].a` 혹은 `lib[xyz].so`와 같은 이름의 파일이 `link_directories`혹은 `target_link_directories`(CMake 3.13부터 지원)로 지정한 폴더들 안에 존재한다
    1. `[xyz].lib` 혹은 `[xyz].dll`와 같은 이름의 파일이 시스템 라이브러리 폴더에 존재한다


이것이 의미하는 바는, `xyz`라는 이름만으로는 **혼동의 여지가 있다**는 것입니다.
앞선 OpenMP 예시에서 `::`을 사용한 부분이 있었습니다. 이를 (C++ 개발자라면 모두가 친숙한) Namespace라고 합니다. 아래와 같이 사용할 수 있습니다.

```cmake
# ...
elseif(NOT WIN32)
    set_target_properties(custom_mp
    PROPERTIES
        INTERFACE_LINK_LIBRARIES    OpenMP::OpenMP_CXX
    )
endif()
```

이런 ALIAS를 발견한다면, CMake와 읽는이 모두 이것이 Target의 이름이라고 확신할 수 있습니다. Alias Target은 아래와 같이 매우 간단한 방법으로 추가할 수 있습니다.

```cmake
# https://github.com/catchorg/Catch2/blob/master/CMakeLists.txt

# ...
add_library(Catch2 INTERFACE)
# ...
add_library(Catch2::Catch2 ALIAS Catch2)
# ...
```

### Custom Target

#### [`add_custom_target`](https://cmake.org/cmake/help/latest/command/add_custom_target.html)

이 함수와 `add_custom_command`의 결정적인 차이점은 `add_custom_command`은 파일을 생성하기 위한(`OUTPUT`) 기능이고, `add_custom_target`는 여러 명령을 (이름으로) 묶어서 실행하기 위한 기능이라는 점입니다.
그리고 Target이기 때문에 `add_dependencies`의 대상이 될 수 있다는 점이 있습니다.

예를 들어, html파일을 하나 다운로드 받아, 이를 첨부하여 메일을 보내는 일은 아래처럼 작성할 수 있습니다. 두 기능 모두 cURL을 사용해 진행하겠습니다.

```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.8) # <--- Just any of 3.x will be fine

add_custom_target(get_index_html
    COMMAND curl -L "https://cmake.org/cmake/help/latest/" 
                -o "index.html"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(mail_index_html
    COMMAND curl --url 'smtps://smtp.gmail.com:465'
                --ssl-reqd  --mail-from 'sender@mail.com'
                --mail-rcpt 'receiver@mail.com'
                --upload-file "${CMAKE_CURRENT_SOURCE_DIR}/index.html"
                --user 'sender@mail.com:$ENV{sender_password}'
    WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}
)
add_dependencies(mail_index_html get_index_html)
```

굉장히 사악한 행동을 하고 있는데,
자세히 보셨다면 `$ENV{sender_password}`를 사용해 CMake 변수가 아니라 시스템 환경변수를 참조하는 것을 볼 수 있습니다.
아래와 같이 CMake를 호출하기 전에 환경변수의 값을 정해주면 문제가 없습니다.

예를들어, 여러분이 Bash 사용자라면:

```bash
export sender_password="the_real_password"
```

만약 PowerShell 사용자라면:

```ps1
$env:sender_password="the_real_password"
```

처럼 환경변수를 설정할 수 있습니다.

위 예시는 UNIX/Linux cURL을 사용하므로, CMake를 바로 다음에 호출하면 아마 아래와 같은 내용을 보실 수 있을 것입니다.

>
> **주의**: Powershell에서 curl명령은 `Invoke-WebRequest`의 Alias입니다.  
> 빌드를 수행하지 않는 예시이기 때문에 간단히 WSL(Windows Subsystem for Linux)를 사용하면 Linux cURL을 실행할 수 있습니다.
>

```console
$ cmake .
-- The C compiler identification is GNU 9.1.0
-- The CXX compiler identification is GNU 9.1.0
-- Check for working C compiler: /usr/bin/cc
-- Check for working C compiler: /usr/bin/cc -- works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Detecting C compile features
-- Detecting C compile features - done
-- Check for working CXX compiler: /usr/bin/c++
-- Check for working CXX compiler: /usr/bin/c++ -- works
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Configuring done
-- Generating done
```

`mail_index_html`은 Target의 이름이므로, 이를 실행하려면 `--target` 파라미터를 명시하는 것으로 충분합니다.

```bash
cmake --build . --target mail_index_html
```

```console
$ cmake --build . --target mail_index_html
Scanning dependencies of target get_index_html
  % Total    % Received % Xferd  Average Speed   Time    Time     Time  Current
                                 Dload  Upload   Total   Spent    Left  Speed
100  8942  100  8942    0     0   7675      0  0:00:01  0:00:01 --:--:--  7675
Built target get_index_html
Scanning dependencies of target mail_index_html
  % Total    % Received % Xferd  Average Speed   Time    Time     Time  Current
                                 Dload  Upload   Total   Spent    Left  Speed
100  8942    0     0  100  8942      0   2888  0:00:03  0:00:03 --:--:--  2888
Built target mail_index_html
```

sender@mail.com와 receiver@mail.com에 자신의 메일주소와 비밀번호를 입력해서 실행해보시기 바랍니다.

빌드를 마친 뒤 `${CMAKE_INSTALL_PREFIX}`를 Zip으로 압축해서 Maintainer에게 메일로 보낼 수 있다면 멋질 것 같군요.

```bash
cmake --build . --target install
cmake --build . --target mail_build_outputs
```
