
# CMake 튜토리얼 Lv.1

- [프로젝트의 구성](#프로젝트의-구성)
  - [CMake 배치](#cmake-배치)
  - [소스코드 조직화](#소스코드-조직화)
  - [실행 해보기](#실행-해보기)
- [빌드 관계 설정](#빌드-관계-설정)
  - [의존성](#의존성)
  - [Linking](#Linking)
- [플랫폼 대응](#플랫폼-대응)
  - [CMake 변수](#CMake-변수)
  - [조건부 처리](#조건부-처리)
  - [접근법](#접근법)
- [컴파일러 대응](#컴파일러-대응)
  - [컴파일러 검사](#컴파일러-검사)
  - [컴파일 옵션 사용](#컴파일-옵션-사용)
  - [매크로 선언](#매크로-선언)
- [CMake 파일 작성](#CMake-파일-작성)
  - [CMake Module 폴더 만들기](#CMake-Module-폴더-만들기)
- [설치](#설치)
  - [설치 경로 지정](#설치-경로-지정)

## 프로젝트의 구성

CMake는 빌드 시스템에서 필요로 하는 파일을 생성하는데 그 목적이 있습니다.

### CMake 배치

CMake를 사용해 프로젝트를 관리하고자 한다면, 필요/의도에 맞게 CMakeLists.txt 파일을 배치해야 합니다.

일반적으로 Root CMake 파일을 두는 것이 편리합니다.  
많은 경우 소스코드들은 폴더를 사용해 조직화되어 있으므로, 모든 프로젝트들이 **재귀적으로** 폴더 내에 `CMakeLists.txt`를 두고 있으면 관리할 때 `폴더 == 프로젝트`로 생각할 수 있게 됩니다.  

```console
$ tree ./small-project/
./small-project         # Project root folder
├── CMakeLists.txt      # <--- Root CMake
├── include             # header files
│   └── ... 
├── module1             # sub-project
│   ├── CMakeLists.txt
│   └── ... 
├── module2             # sub-project
│   ├── CMakeLists.txt
│   └── ... 
└── test                # sub-project
    ├── CMakeLists.txt
    └── ... 
```


#### [`project`](https://cmake.org/cmake/help/latest/command/project.html)

프로젝트의 이름을 지정할 수 있습니다.  
CMakeLists.txt에 다음과 같이 작성하는 것으로 프로젝트에 이름을 부여하게 됩니다.

```cmake
# CMake에서 주석은 #을 사용해 작성합니다
project(my_project)
```

좀 더 상세하게, 언어와 버전을 명시하는 것도 가능합니다.

```cmake
# 한 줄에 모든 것을 적을 수 있습니다
project(my_project  LANGUAGES CXX   VERSION 1.2.3)

# multi-line으로 작성하는 것 또한 가능합니다
project(my_project  
        LANGUAGES   CXX   
        VERSION     1.2.3
)
```
 
이렇게 `project`를 명시하게 되면 Visual Studio 에서는 같은 이름으로 Solution 파일이 생성됩니다. 즉, `project`만으로는 프로그램을 생성하지 않습니다.
실제로 프로그램을 생성하기 위해서는 `add_executable`, `add_library` 를 사용하여야 합니다.

### 소스코드 조직화

프로그램(exe, lib, dll, a, so, dylib ...)을 만들기 위해서는 컴파일러에게 제공할 소스코드가 필요합니다. 
CMake에게 **소스코드 목록**으로부터 생성할 **프로그램의 타입**을 지시하기 위해 사용하는 함수들이 바로 `add_executable`, `add_library`입니다. `project`는 하나만 가능하지만, 이 함수들은 CMakeList안에서 여러번 사용되기도 합니다. 빌드 결과 생성되는 프로그램의 **이름만 다르다면** 크게 문제되지 않습니다.

#### [`add_executable`](https://cmake.org/cmake/help/latest/command/add_executable.html), [`add_library`](https://cmake.org/cmake/help/latest/command/add_library.html)

아래와 같은 구조로 프로젝트가 구성되었다고 해봅시다. 

```console
$ tree ./project-example/
./project-example
├── CMakeLists.txt
├── include
│   └── ... 
└── src
    ├── CMakeLists.txt
    ├── main.cpp
    ├── feature1.cpp
    ├── feature2.cpp
    ├── algorithm3.cpp
    └── data_structure4.cpp
```

우선 Root CMakeList(`project-example/CMakeLists.txt`)가 아니라 src/ 폴더에 있는 CMakeList(`project-example/src/CMakeLists.txt`)부터 살펴보겠습니다.

만약 src 폴더에 있는 모든 .cpp 파일들이 실행 파일(exe)을 만든다면, 아래와 같은 내용이 작성되어야 합니다.

```cmake
# project-example/src/CMakeLists.txt 
# case 1

add_executable(my_exe   # 이후에 나오는 .cpp 파일을 사용해 .exe를 생성한다
    main.cpp
                        # 개행을 여러번 하여도 문제되지 않습니다.
    feature1.cpp
    feature2.cpp
    algorithm3.cpp      # 상대 경로로 소스 코드를 찾아냅니다. 
                        # 현재 사용중인 CMakeList의 위치를 기준으로
                        # 경로를 지시해야 합니다
    data_structure4.cpp
    # ... 
)
```

만약 라이브러리를 만든다면, 아래와 같은 내용이 작성되어야 합니다.

```cmake
# project-example/src/CMakeLists.txt 
# case 2

add_library(my_lib      # 이후에 나오는 .cpp 파일을 사용해 라이브러리를 생성한다
    main.cpp
    feature1.cpp
    # ...
)
```

라이브러리의 링킹의 형태를 명시하지 않는다면 여기서 생성되는 라이브러리는 프로젝트 생성시 `BUILD_SHARED_LIBS` 변수를 따라서 결정됩니다. 물론 직접 명시할 수도 있습니다.

```cmake
# project-example/src/CMakeLists.txt 
# case 2.1, 2.2

add_library(my_archive  STATIC  # 정적 링킹 라이브러리(.a, .lib)
    main.cpp
    feature1.cpp
    # ...
)

add_library(my_shared_object  SHARED  # 동적 링킹 라이브러리(.so, .dll)
    main.cpp
    feature1.cpp
    # ...
)
```

`STATIC`, `SHARED`가 대문자인 점에 주의하십시오. **CMake파일은 대소문자를 신경써서 사용해야만 합니다**. CMake 함수들은 앞서 예시처럼 소문자를 사용해도, `PROJECT`, `ADD_EXECUTABLE`처럼 대문자로 작성하여도 문제없이 동작합니다. 하지만 `SHARED`처럼 일부 예약된 단어(sub-command)들은 대문자만을 사용해야 합니다. 

**가독성**을 고려하여 일관성있게 작성하는 것이 좋습니다. CMake기반 프로젝트에서는 개발자들이 서로의 CMakeList를 **주의 깊게** 살펴보는 경우가 많습니다.  
보고 배울만한 CMake 사례를 모아두십시오. 특정한 저장소나 프로젝트를 따라해도 좋습니다.

#### [`add_subdirectory`](https://cmake.org/cmake/help/latest/command/add_subdirectory.html)

이번에는 Root CMakeList(`project-example/CMakeLists.txt`)를 살펴보겠습니다. 이미 어떻게 프로그램을 생성할 것인지는 앞서 src 폴더에서 작성한 CMakeLists가 잘 처리하고 있기 때문에, Root CMakeList에서는 이를 그대로 사용할 것입니다.

```console
$ tree ./project-example/
./project-example
├── CMakeLists.txt          # <---- project
├── include
└── src
    ├── CMakeLists.txt      # <---- add_executable/add_library
    └── ... 
```

간단히 `add_subdirectory`와 폴더 이름을 주는 것으로 이를 달성할 수 있습니다.  
많은 경우 CMakeList는 아래와 같이 재귀적으로, 즉 `add_subdirectory`만으로 프로젝트를 (File Tree를 따라) 조직화 합니다.

```cmake
# project-example/CMakeLists.txt
project(my_project  LANGUAGES CXX   VERSION 1.2.3)

add_subdirectory(src)       # 상대경로
```

##### 외부 경로 가져오기

하지만 경우에 따라선 필요한 CMakeList가 Root CMakeList의 하위에 없을 수도 있습니다. 이런 경우를 Out-of-tree build라고 합니다.

아래와 같은 경우를 생각해봅시다.

```console
$ tree ./out-of-tree
./out-of-tree/
├── current-project
│   └── CMakeLists.txt
└── far-far-away
    ├── CMakeLists.txt
    └── algorithm1.cpp
```

여기서 `current-project/CMakeLists.txt`는 `add_subdirectory`로 하위 디렉터리가 아닌 다른 곳을 참고 하고 있습니다. 

```cmake
# current-project/CMakeLists.txt

cmake_minimum_required(VERSION 3.10)    # CMake 버전을 명시
project(my_project)

add_subdirectory(../far-far-away)       # 상대경로를 사용해 접근
```

이대로 CMake를 실행하면 다음과 같은 오류를 발생시킵니다.

```console
# ...
-- Detecting CXX compiler ABI info - done
-- Detecting CXX compile features
-- Detecting CXX compile features - done
CMake Error at CMakeLists.txt:6 (add_subdirectory):
  add_subdirectory not given a binary directory but the given source
  directory "X:/Develop/out-of-tree/far-far-away" is not a subdirectory of
  "X:/Develop/out-of-tree/current-project".  When specifying an out-of-tree
  source a binary directory must be explicitly specified.
-- Configuring incomplete, errors occurred!
```

다행히도, `add_subdirectory`를 하는 CMakeList에서 경로를 조정하는 방법으로 사용할 수 있습니다. When specifying an 'out-of-tree' source a binary directory must be explicitly specified 문장을 다시 한번 보시기 바랍니다.  
[**binary directory를 명시할 것을 요구하고 있습니다**](https://cmake.org/cmake/help/latest/command/add_subdirectory.html).

```cmake
# current-project/CMakeLists.txt

cmake_minimum_required(VERSION 3.10)    # CMake 버전을 명시
project(my_project)

add_subdirectory(../far-far-away            # CMakeList가 위치한 source dir
                 ./build/far-far-away-dir   # 빌드 결과물을 배치할 binary dir을 지정
)
```
CMake 문서의 지시를 따라 위와 같이 수정하면 아래와 같은 결과를 얻습니다.

```console
# ...
-- Detecting CXX compiler ABI info - done
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Configuring done
-- Generating done
-- Build files have been written to: X:/Develop/out-of-tree/current-project
```

오류가 사라진 것을 확인할 수 있습니다.
폴더를 열어 확인해보면 지정한 build directory 경로에 빌드 파일들이 생성되어 있습니다.

```console
PS X:\Develop\out-of-tree> tree .
Folder PATH listing for volume Drive_X
Volume serial number is D688-4B7E
X:\DEVELOP\OUT-OF-TREE
├─current-project
│  ├─build
│  │  └─far-far-away-dir    # <----- ./build/far-far-away-dir
│  │      └─CMakeFiles
│  └─CMakeFiles
│      ├─3.10.1
│      ├─a8084ac71a5a995e5212369c2e1624fc
│      └─CMakeTmp
└─far-far-away
```

#### 예시: 단일 실행 파일 / 라이브러리

당연하게도, 간단한 프로젝트들이 모여야 큰 프로젝트를 이루게 됩니다. C++로 하나의 프로그램을 만든다면 아마 아래와 같이 CMake 프로젝트를 구성해야 할 것입니다.

```console
$ tree ./small-project/
./small-project
├── CMakeLists.txt
├── include
│   └── ... 
└── src
    ├── CMakeLists.txt
    └── ... 
```

#### 예시: 라이브러리 + 실행 파일 각각 1개씩

만약 라이브러리를 만들었다면, 테스트와 같이 라이브러리 코드를 실행시켜 볼 수 있는 추가 프로젝트가 필요할 것입니다. 이런 경우 또 다른 sub-project를 추가 하면 됩니다.

```console
$ tree ./project-with-test/
./project-with-test
├── CMakeLists.txt
├── include
│   └── ... 
├── src
│   ├── CMakeLists.txt
│   └── ... 
└── test
    ├── CMakeLists.txt
    └── ... 
```

#### 예시: 2개 라이브러리 + 1개 실행 파일

라이브러리를 하나만 사용하라는 법은 없죠.

```console
$ tree ./multiple-library-project
./multiple-library-project
├── CMakeLists.txt
├── include
│   └── ... 
├── module1
│   ├── CMakeLists.txt
│   └── ... 
├── module2
│   ├── CMakeLists.txt
│   └── ... 
└── test
    ├── CMakeLists.txt
    └── ... 
```

#### 예시: 계층화된 라이브러리 (depth 2) + 1개 실행 파일

OpenCV와 같이 좀 더 복잡하게 구성할 수도 있습니다. 다수의 모듈들을 한번 더 조직화 하기도 합니다. 하위 경로에 있기만 하다면 `add_subdirectory`를 사용하는데 크게 문제될 일이 없습니다.
출처에 따라서 Internal/External로 두기도 하고, 목적에 따라서 특별한 이름을 붙일 수도 있습니다.

```console
$ tree ./huge-project
./huge-project
├── CMakeLists.txt
├── cmake
│   └── get-some-package.cmake
├── include
│   └── ... 
├── modules
│   ├── guideline_support_library
│   │   ├── CMakeLists.txt
│   │   └── ...
│   ├── fmt
│   │   ├── CMakeLists.txt
│   │   └── ...
│   ├── grpc
│   │   ├── CMakeLists.txt
│   │   └── ...
│   └── ... 
├── src
│   ├── CMakeLists.txt
│   └── ... 
└── test
    ├── CMakeLists.txt
    └── ... 
```

git을 사용하는 프로젝트라면 submodule을 추가하면서 함께 조정하면 될 것입니다.

### 실행 해보기

앞서까지는 CMake를 실행하는 방법을 언급하지 않았습니다.  
CMake가 빌드 시스템에 맞는 파일들(sln, vcxproj, MakeFiles, ninja ...)을 생성하기 위해선 당연하게도 "생성기"를 명시해야 합니다. 이 생성기는 플랫폼에 맞는 개발 툴을 지정하게 됩니다. [이 과정은 Configure / Generate 라는 2 단계로 구성됩니다.](https://cgold.readthedocs.io/en/latest/tutorials/cmake-stages.html)

* Configuration : `CMakeLists.txt` 파일 해석
* Generation: 해석 결과를 바탕으로 Project 파일 생성

아래는 커맨드라인으로 각 플랫폼에서 주로 사용되는 IDE에 맞는 파일을 생성하는 것을 보여줍니다  

##### Windows

PowerShell에서는 ""를 사용해 문자열을 명시한 점에 주의하십시오

```ps1
# Generate for VS
cmake ./Path/to/Root/ -G "Visual Studio 15 2017 Win64"
```

##### Unix/Linux
```sh
# Generate for Ninja
cmake ./Path/to/Root/ -G Ninja
```

##### MacOS

```sh
# Generate for XCode
cmake ./Path/to/Root/ -G XCode
```


#### [빌드 시스템](https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html)/[툴체인](https://cmake.org/cmake/help/latest/manual/cmake-toolchains.7.html#cmake-toolchains-7) 지정

거듭 강조하자면, CMake의 목적은 파일을 생성하는데 있으며, 프로그램 빌드를 하지는 않습니다.  
다만 아래와 같이 커맨드라인에서 `-G` 옵션으로 지정한 빌드시스템을 호출하도록 명령할 수는 있습니다.

Powershell/Bash 모두 동일합니다.
```ps1
cmake --build . # ... 지정된 툴을 사용해 빌드를 진행한다 ...
cmake --build . --parallel          # 병렬 빌드
cmake --build . --target install    # 빌드에 성공하면 설치까지 진행한다
cmake --build . --config debug      # Debug로 빌드
```

**툴체인 파일**은 CMake가 지원하는 다양한 기능들을 사용해 빌드를 수행할 수 있도록 미리 지정된 파일을 의미합니다. 

대표적으로 Android NDK에서는 여러 아키텍처로의 크로스 컴파일에 필요한 설정들이 작성된 android.toolchain.cmake 파일이 함께 제공되며, CMake를 사용한 빌드를 수행시에 Gradle에 의해서 자동으로 지정됩니다.  
iphone을 대상으로 하는 경우에는 https://github.com/leetal/ios-cmake 를 사용해 XCode 프로젝트를 생성하기도 합니다.

다르게는 https://github.com/Microsoft/vcpkg 와 같이 라이브러리 탐색에 특화된 툴체인 파일을 사용하는 경우도 있습니다.

```ps1
# ... Vcpkg에서 제공하는 cmake 툴체인의 경로로 제공한다 ...
cmake ../ -G "Visual Studio 15 2017 Win64" -DCMAKE_TOOLCHAIN_FILE="X:\Develop\vcpkg\scripts\buildsystems\vcpkg.cmake"
```

## 빌드 관계 설정

### 의존성

프로젝트 내에서 여러 라이브러리를 빌드한다면, 그리고 서로 의존성이 있다면 이를 제어하는 것도 가능합니다. 이 과정은 보통 해당 sub-project들을 모두 확인할 수 있는 CMakeList에서 수행하게 됩니다.

#### [`add_dependencies`](https://cmake.org/cmake/help/latest/command/add_dependencies.html)

의존성은 `add_dependencies` 함수를 사용해 명시하게 됩니다. 이름에서 알 수 있듯이 다수를 지정할 수 있습니다.

아래와 같은 형태로 프로젝트가 구성되었다고 가정하겠습니다.

```console
$ tree ./multiple-library-project
./multiple-library-project
├── CMakeLists.txt      # <--- Root CMakeList
├── include
│   └── ... 
├── module1
│   ├── CMakeLists.txt
│   └── ... 
├── module2
│   ├── CMakeLists.txt
│   └── ... 
└── test
    ├── CMakeLists.txt
    └── ... 
```

`module2`가 `module1`의 기능을 사용하고, `test`는 이 둘의 기능을 모두 사용한다면 이는 Root CMake에서 다음과 같이 명시할 수 있습니다.

```cmake
# multiple-library-project/CMakeLists.txt

add_subdirectory(module1)
add_subdirectory(module2)
add_subdirectory(test)

# dependency: from -> { to }
add_dependencies(module1 module2)            # `module1` requires `module2`
add_dependencies(test    module1 module2)    # `test` requires `module1` & `module2`
```

이를 바탕으로 CMake에서 프로젝트가 생성되면, 그 프로젝트는 의존성을 고려하여 `module1`, `module2`, `test` 순서로 빌드를 진행하게 됩니다.

#### [별명 붙이기](https://cmake.org/cmake/help/latest/command/add_library.html#alias-libraries)

라이브러리들의 수가 많아지거나 경우에 따라 달라지면 별명을 붙여서 상위 CMakeList에서 좀 더 편하게 사용할 수 있도록 할수도 있습니다.

```cmake
# sub-project CMakeList
add_library(my_custom_logger_lib 
    # ...
)

add_library(module::logger ALIAS my_custom_logger_lib)
```

디버그 모드와 릴리즈 모드에서 라이브러리 이름이 바뀌는 경우 이는 굉장히 유용한 기능이 됩니다.

```cmake
# higher-project CMakeList

add_subdirectory(some_exe)
add_subdirectory(custom_logger)

add_dependencies(some_exe   module::logger) # Use with alias
```

### Linking

빌드 순서를 제어하기 위해서 `add_dependencies`를 사용했다면, 프로그램 생성시에(링킹에) 필요한 (Symbol Table과 같은) 정보들이 공유되도록 하기 위해서는 `target_link_libraries`를 사용하게 됩니다.

이 문서에서 Target이라는 용어가 처음 사용되었는데, 이는 `add_executable` 혹은 `add_library`를 사용해 생성한 대상을 의미합니다. 빌드 시스템 파일 생성의 단위이기도 합니다

#### [`target_link_libraries`](https://cmake.org/cmake/help/latest/command/target_link_libraries.html)

```cmake
add_library(my_custom_logger_lib 
    # ...
)

target_link_libraries(my_custom_logger_lib # 
PUBLIC
    spdlog fmt
PRIVATE
    utf8proc
)
```

특히 이 함수는 Target의 의존성을 전파시키는 역할도 수행합니다. 위와 같은 경우, `PRIVATE`와 `PUBLIC`을 사용해 이를 제어하고 있습니다. C++ class의 멤버 접근 한정자(Access Qualifier)와 유사하게 생각할 수 있습니다.  
위와 같이 작성하면 다른 프로젝트에서 `my_custom_logger_lib`을 link하는 경우, `spdlog`와 `fmt`에 있는 **헤더파일을 include하기 위한 경로**와 **빌드 결과 생성되는 라이브러리**에 접근할 수 있게 됩니다. 하지만 `PRIVATE`로 선언된 `utf8proc`은 이와 같은 정보를 차단합니다.

```cmake
add_executable(some_test_program)
    # ...
)

target_link_libraries(some_test_program
PUBLIC
    my_custom_logger_lib  # spdlog 와 fmt 이 적혀있지 않지만 자동으로 추가된다
)
```

의존성이 공유되어야 하는 경우, 혹은 기타 라이브러리와 함께 사용하는 라이브러리라면 `PUBLIC`, 내부 구현에만 사용되고 공개되지 않는 경우라면 `PRIVATE`에 배치하는 것이 적합합니다. 

## 플랫폼 대응

하지만 크로스 플랫폼은 아주아주 힘든 일입니다. CMake에서도 이 문제에 대응해보겠습니다

### CMake 변수

프로그래머라면 변수에 익숙할 것입니다. CMake에서도 변수가 있으며, 단순한 Boolean, 문자열, 파일 경로, 리스트 등을 표현할 수 있습니다. 이 튜토리얼에서는 이 중 빈번하게 사용되는 변수들을 짚고 넘어가겠습니다

#### [`set`](https://cmake.org/cmake/help/latest/command/set.html) / [`unset`](https://cmake.org/cmake/help/latest/command/unset.html)

변수는 `set`을 사용해서 생성하고, `unset`을 사용해서 제거할 수 있습니다. 변수는 문자열 기반이며, bash 쉘 프로그래밍처럼 사용할 수 있습니다. 다만 **문자열에 대한 참조**처럼 사용된다는 특이점이 있습니다.

```cmake
set(CMAKE_BUILD_TYPE Debug)

message(STATUS CMAKE_BUILD_TYPE)                     # -- CMAKE_BUILD_TYPE
message(STATUS ${CMAKE_BUILD_TYPE})                  # -- Debug
message(STATUS "Configuration: ${CMAKE_BUILD_TYPE}") # -- Configuration: Debug
```

변수의 값을 사용하기 위해 `${}`를 사용한 점을 주의깊게 보시길 바랍니다.
`message`는 문자열을 출력하므로 변수를 그대로 주지 않고 한번 참조하여 문자열로 변환한 것입니다. 그대로 변수 이름만을 제공할 경우 변수 이름을 문자열로 바꿔 출력하는 것을 확인할 수 있습니다

### 조건부 처리

#### `if`/`elseif`/`else`/`endif`

플랫폼이 다르면 System API, 혹은 같은 API더라도 구현 형태나 지원 범위가 상이할 수 있습니다. 조건부 분기문을 사용해 플랫폼에 맞게 미리 작성된 라이브러리를 선택하도록 하면 상대적으로 부담이 줄어들 것입니다.

CMake는 플랫폼 관련 변수들을 제공하고 있으며, Android 혹은 iOS로 크로스 컴파일을 하기 위해 CMake Toolchain을 사용한 경우 그 값을 참고해 처리를 다르게 할 수 있습니다.

```cmake
# *현재* CMake가 실행되는 시스템을 알려진 변수들로 확인하는 방법

# wrapper::system 같은 별명을 붙이면 상대적으로 편해진다
if(WIN32)      
    add_subdirectory(external/winrt)
    add_subdirectory(impl/win32)
elseif(APPLE)
    add_subdirectory(impl/posix)
    # additional implementation for MacOS
    add_subdirectory(impl/macos)
elseif(UNIX)
    add_subdirectory(impl/posix)
    # additional implementation with Linux API
    if(${CMAKE_SYSTEM} MATCHES Linux)
        add_subdirectory(impl/linux)
    endif()
else()
    # 지원하지 않음.
    # android.toolchain.cmake 혹은 ios.toolchain.cmake 에서 지정하는 변수들
    if(ANDROID OR IOS)
        message(FATAL_ERROR "No implementation for the platform")
    endif()
    # ...
endif()
```

가장 윗 줄에 주석으로 적은 것처럼 **현재 시스템**을 변수로 알려주는 것이라는 점에 주의하시기 바랍니다. 크로스 컴파일을 위한 라이브러리라면 변수를 검사하는 순서, 조건문에 주의를 기울여야 합니다.

### 접근법

구현이 다르더라고, 공통된 인터페이스(헤더파일)가 있다면 이들을 한곳에 모아놓는 것이 타당할 것입니다. 문서 초반부에 프로젝트 예시로 include 폴더가 꾸준히 나타났다는 것을 기억하십니까?

#### [`target_include_directories`](https://cmake.org/cmake/help/latest/command/target_include_directories.html)

헤더파일들이 위치한 폴더는 제각기 다를 수 있습니다. C 혹은 C++ 프로젝트에서 외부에 공개되는 헤더파일과 공개되지 않는 헤더파일이 다른 위치에 배치되는 것은 흔한 일입니다.

`target_include_directories`는 Target에서 헤더파일을 찾기 위해 사용하는 폴더를 지정하는 함수이며, 이곳에 위치한 헤더파일들은 `#include <my_interface.h>`와 같이 `<>` 형태로 include하는 것이 가능합니다.

아래와 같은 프로젝트 구조를 생각해봅시다.

```console
$ tree ./some-huge-project
./some-huge-project
├── CMakeLists.txt
├── include                 # <--- 공용 헤더 파일
│   ├── system_wrapper.h
│   └── ... 
├── impl
│   ├── win32
│   │   ├── CMakeLists.txt
│   │   ├── include         # <--- 구현 헤더 파일
│   │   │   ├── pch.h
│   │   │   └── my_win32.h
│   │   └── ...
│   ├── posix
│   │   ├── CMakeLists.txt
│   │   ├── include         # <--- 구현 헤더 파일
│   │   │   └── my_posix.h
│   │   └── ...
│   └── ... 
├── src
│   ├── CMakeLists.txt      # <--- system 라이브러리를 사용하는 프로젝트
│   └── ... 
├── ... 
```

우선 Win32 하위 프로젝트에 있는 CMakeLists 부터 살펴보겠습니다.

```cmake
# some-huge-project/impl/win32/CMakeLists.txt

add_library(my_win32_wrapper
    src/i-love-win32.cpp
)
add_library(wrapper::system ALIAS my_win32_wrapper)

target_include_directories(my_win32_wrapper
PUBLIC
    ${CMAKE_SOURCE_DIR}/include
                # CMAKE_SOURCE_DIR 는 최상위 CMakeLists.txt가 위치한 폴더를 의미한다.
                # 이 프로젝트에서는 some-huge-project/
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include     
                # include 폴더를 절대경로를 사용해 접근
                # CMAKE_CURRENT_SOURCE_DIR 는 현재 해석 중인 CMakeLists.txt가 위치한 폴더를 의미한다.
                # 즉, 경로는 some-huge-project/impl/win32
)

```

POSIX API에 맞춘 프로젝트의 CMakeList는 이렇게 작성할 수 있습니다.

```cmake
# some-huge-project/impl/posix/CMakeLists.txt

add_library(my_posix_wrapper
    src/i-love-posix.cpp
)
add_library(wrapper::system ALIAS my_posix_wrapper)

target_include_directories(my_posix_wrapper
PUBLIC
    ${CMAKE_SOURCE_DIR}/include
                # win32와 header 파일들을 공유한다. 
                # 예컨대 some-huge-project/include에 위치한 system_wrapper.h
PRIVATE
    include     # include 폴더를 상대경로 접근
                # some-huge-project/impl/posix/include를 의미한다
)
```

예시에서 본것과 같이 이 함수는 `target_link_libraries`처럼 `PUBLIC`,`PRIVATE`을 지정할 수 있으며, PUBLIC에 위치한 폴더들은 경로가 자동으로 전파됩니다.

```cmake
# some-huge-project/src/CMakeLists.txt

add_executable(my_system_utility
    some.cpp
    source.cpp
    files.cpp
)

target_link_libraries(my_system_utility
PRIVATE
    wrapper::system # wrapper 라이브러리들의 ALIAS
                    # target_include_directories에 PUBLIC으로 명시된
                    # ${CMAKE_SOURCE_DIR}/include 폴더를 자동으로 접근할 수 있게 된다
)
```


#### [`target_sources`](https://cmake.org/cmake/help/latest/command/target_sources.html)

`add_executable`과 `add_library`의 한계는 소스 파일 목록을 한번에 결정해서 전달해야 한다는 점입니다. 이는 Target들이 CMakeList 파일의 끝부분에 나타나게 만들며, 2.x 버전 CMake들이 사용했던 방법처럼 List 변수를 사용해 소스파일 목록을 만들어야 하는 불편함이 있습니다.

CMake 3.x에서는 Target에 소스파일을 '추가'할 수 있도록 `target_sources`를 사용할 수 있습니다.

```cmake
# preview 가 구현된 소스파일을 추가할지 결정하는 변수
set(USE_PREVIEW true)

# target: my_program
add_executable(my_program
    main.cpp
    feature1.cpp
)

# ...
# add_dependencies
# set_target_properties
# target_include_directories
# target_link_libraries
# ...

target_sources(my_program
PRIVATE
    feature2.cpp
)
if(USE_PREVIEW)
    target_sources(my_program
    PRIVATE
        feature3_preview.cpp
    )
else()
```

## 컴파일러 대응

플랫폼이 달라지면 컴파일러도 달라질 수 있습니다. 컴파일러가 달라지면 프로그램 생성에 사용할 수 있는 컴파일 옵션들이 달라지게 됩니다. 이 튜토리얼에서는 간단히 Warning, Optimization를 다르게 적용하는 예시를 보이겠습니다

### 컴파일러 검사

앞서서 Platform을 검사한 것처럼, Compiler와 관련해 미리 지정된 변수들이 존재합니다.

#### 관련 CMake 변수들

아래의 내용을 CMakeList에 추가한 이후 실행해보시기 바랍니다. 

- `CMAKE_CXX_COMPILER_ID`: 컴파일러의 이름
- `CMAKE_CXX_COMPILER_VERSION`: 컴파일러의 버전
- `CMAKE_CXX_COMPILER`: 컴파일러 실행파일의 경로


```cmake
message(STATUS "Compiler")
message(STATUS " - ID       \t: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " - Version  \t: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS " - Path     \t: ${CMAKE_CXX_COMPILER}")
```

Windows에서 별다른 조작 없이 Visual Studio 15 2017 Win64를 Generator로 지정하면, 아래와 같이 출력될 것입니다. (Community 버전 기준)

```console
-- Compiler
--  - ID        : MSVC
--  - Version   : 19.16.27024.1
--  - Path      : C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC/Tools/MSVC/14.16.27023/bin/Hostx86/x64/cl.exe
````

컴파일러를 Clang-cl을 명시하면 아래와 같은 출력을 확인할 수 있습니다.

```console
-- Compiler
--  - ID       	: Clang
--  - Version  	: 7.0.0
--  - Path     	: C:/Program Files/LLVM/bin/clang-cl.exe
```

Mac OS에서는 아래와 AppleClang에 대한 정보를 보여줍니다.

```console
-- Compiler
--  - ID       	: AppleClang
--  - Version  	: 9.1.0.9020039
--  - Path     	: /Applications/Xcode-9.4.1.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++
```

아래와 같이 CMake 내에서  컴파일러에 따라서 처리를 다르게 할 수 있습니다.

```cmake
if(MSVC)        # Microsoft Visual C++ Compiler
    # ...
elseif(${CMAKE_CXX_COMPILER_ID} MATCHES Clang)  # Clang + AppleClang
    # ...
elseif(${CMAKE_CXX_COMPILER_ID} MATCHES GNU)    # GNU C Compiler
    # ...
endif()
```

선술한 플랫폼 변수들을 함께 고려하면 조합이 많이 발생할 수 있기 때문에, CMake 파일들을 간결히 유지하려면 플랫폼에 따라서 특정 컴파일러만을 지원하는 것이 타당할 것입니다.

### 컴파일 옵션 사용

#### `include(CheckCXXCompilerFlag)`

컴파일러를 식별할 수 있게 되었으니 이제 컴파일러 옵션을 지정해줄 차례입니다. 하지만 그 전에 컴파일러가 해당 옵션을 지원하는지 검사가 필요한 경우도 있습니다.
CMake의 기본 모듈들 중에는 이를 지원하는 `CheckCXXCompilerFlag`라는 모듈이 있습니다.

CMake Module은 간단히 말하자면 미리 작성된 CMake 파일이라고 생각할 수 있습니다. 이런 파일들은 각 `project`들마다 각각 cmake 폴더를 만들어 배치하는 경우가 많습니다.

```console
tree -L 2 .
.
├── CMakeLists.txt
├── cmake             # <---- 이 프로젝트에서 필요한 cmake 파일들을 모아둔다
│   ├── display-compiler-info.cmake
│   └── test-cxx-flags.cmake
├── include
│   └── ...
├── modules
│   ├── ...
│   └── ...
├── scripts
│   └── ...
├── src
└── test
```

test-cxx-flag.cmake 파일의 내용은 아래와 같습니다. CMake Module에 대한 내용은 후술할 것이므로, CMake를 처음 접하였다면 **같은 내용을 CMakeList에 붙여넣기하는 것과 같은 효과가 생긴다**고 생각하면 되겠습니다.

```cmake
# test-cxx-flags.cmake
#
# `include(cmake/check-compiler-flags.cmake)` from the root CMakeList
#
include(CheckCXXCompilerFlag)

# Test latest C++ Standard and High warning level to prevent mistakes
if(MSVC)
    check_cxx_compiler_flag(/std:c++latest  cxx_latest          )
    check_cxx_compiler_flag(/W4             high_warning_level  )
elseif(${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
    check_cxx_compiler_flag(-std=c++2a      cxx_latest          )
    check_cxx_compiler_flag(-Wall           high_warning_level  )
elseif(${CMAKE_CXX_COMPILER_ID} MATCHES GNU)
    check_cxx_compiler_flag(-std=gnu++2a    cxx_latest          )
    check_cxx_compiler_flag(-Wextra         high_warning_level  )
endif()
```

이같은 내용을 추가하여 CMake를 실행하면 다음과 같은 내용이 나타날 것입니다. [`check_cxx_compiler_flag`](https://cmake.org/cmake/help/v3.13/module/CheckCXXCompilerFlag.html)는 해당 컴파일 옵션을 사용할 수 있으면 두번째 인자에 사용한 이름으로 변수를 생성합니다.

```console
# ...
-- Compiler
--  - ID        : Clang
--  - Version   : 6.0.0
--  - Path      : /usr/bin/clang-6.0
# ...
-- Performing Test cxx_latest
-- Performing Test cxx_latest - Success
-- Performing Test high_warning_level
-- Performing Test high_warning_level - Success
# ...
```

위와 같은 경우, clang-6.0은 `-std=c++2a`, `-Wall`를 모두 사용할 수 있으므로 `cxx_latest`, `high_warning_level` 변수는 모두 true(1)값을 가질 것입니다. 특정 컴파일 옵션을 사용할 수 없는 경우 경고하거나 우회하는 CMakeList를 상상해보시기 바랍니다.

#### [`target_compile_options`](https://cmake.org/cmake/help/latest/command/target_compile_options.html)

이제 컴파일 옵션을 사용할 준비과 되었으므로, 간단히 Target에서 사용할 컴파일 옵션을 지정하는 예시를 보이겠습니다.

```cmake
if(MSVC)
    target_compile_options(my_modern_cpp_lib 
    PUBLIC
        /std:c++latest /W4  # MSVC 가 식별 가능한 옵션을 지정
    )
else() # Clang + GCC
    target_compile_options(my_modern_cpp_lib
    PUBLIC
        -std=c++2a -Wall    # GCC/Clang이 식별 가능한 옵션을 지정
    PRIVATE
        -fPIC 
        -fno-rtti 
    )
endif()
```

이번에도 `PUBLIC`, `PRIVATE`으로 컴파일 옵션을 전파시킬 수 있습니다. 즉, my_modern_cpp_lib를 `target_link_libraries`로 사용하는 모든 Target들은 C++ latest, Warn All 옵션으로 빌드가 수행 됩니다.  
하지만 옵션의 중복을 걱정할 필요는 없습니다. **중복되는 옵션은 CMake에서 자동으로 하나로 합쳐서 적용하게 됩니다.**

### 매크로 선언

#### [`target_compile_definitions`](https://cmake.org/cmake/help/latest/command/target_compile_definitions.html)

최신 C++에서는 Macro를 대체할 방법으로 `enum class`, `constexpr`등이 있습니다만, 여전히 Macro에 의존하고 있는 코드가 많은 것 또한 사실입니다. 하지만 수십, 혹은 수백개의 소스 파일에 Macro를 선언하려면 시간이 많이 들뿐만 아니라 이후에 수정하기도 번거로울 것입니다.

```cmake
if(MSVC)
    # 묵시적으로 #define을 추가합니다 (컴파일 시간에 적용)
    target_compile_definitions(my_modern_cpp_lib
    PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX    # numeric_limits를 사용할때 방해가 되는
                    # max(), min() Macro를 제거합니다
        _CRT_SECURE_NO_WARNINGS 
                    # Visual Studio로 C++에 입문했다면 한번쯤 만나본 녀석일 겁니다
                    # 오래된 코드를 위한 프로젝트라면 선택의 여지가 없을 수도 있겠죠...
    )
endif()
```

물론 여기에도 `PUBLIC`, `PRIVATE`가 있습니다. 아마 처음부터 읽으셨다면 어떤 기능을 하는지 더는 설명이 필요 없을 것입니다. **다만 이 방법으로 추가하는 Macro는 소스코드에 보이지 않기 때문에 프로젝트를 가져다 쓰는 사람이 찾아내기 어려울 수 있습니다**. 평시에 신중하게, 주의하면서 사용하는게 좋을 것입니다.

## CMake 파일 작성

변수와 조건문에 대해서 배우고 나면 보통 함수에 대해서 배우게 됩니다. **안타깝게도 이 튜토리얼 CMake Macro와 CMake Function에 대해서는 생략할 것입니다**. 대신, 앞서 CheckCXXCompilerFlag와 같은 형태의 CMake Module에 대해서는 짚고 넘어가겠습니다.

### CMake Module 폴더 만들기

#### `.cmake` & [`include`](https://cmake.org/cmake/help/latest/command/include.html)

CMake는 나름의 문법과 처리방식이 있기 때문에, 전용 확장자가 없는게 더 이상할 것입니다. 최초의 Root CMakeList 호출이나 `add_subdirectory`는 CMakeLists.txt를 사용하지만, 그렇지 않은 경우라면 보통 `.cmake`파일을 사용하게 됩니다.  
앞서 잠깐 언급했던 vcpkg.cmake와 같이 Toolchain파일들이 이런 확장자를 가지고 있었던 것을 기억해주시기 바랍니다.

아래와 같은 프로젝트를 가정해봅시다.
```console
tree -L 2 .
.
├── CMakeLists.txt  # <---- Root
├── cmake
│   ├── display-compiler-info.cmake # <---- going to `include`
│   └── test-cxx-flags.cmake
├── include
│   └── ...
├── src
│   ├── CMakeLists.txt  # <---- going to `add_subdirectory`
│   └── ...
└── test
    ├── CMakeLists.txt  # <---- going to `add_subdirectory`
    └── ...
```

`add_subdirectory`는 기본적으로 함수(서브루틴)처럼 동작한다고 할 수 있습니다. **독립적으로 CMake 변수들을(유효범위) 가지고**, 별도로 지시하지 않는 한 상위 CMakeList의 변수를 변경하지 않습니다. sub-project에서 벗어나면 그 변수들은 사라집니다.  
반면 `include`는 C++ 코드에서 `inline`을 지정하는 것과 유사합니다. `include`를 통해 실행되는 cmake는 **현재 CMakeLists의 변수들에 그대로 접근**할 수 있습니다. 물론 새로운 변수를 추가할 수도 있습니다.

```cmake
# Root/CMakeLists.txt
project(my_new_project)

# .cmake의 내용을 복사-붙여넣기 한 것처럼 동작한다
include(cmake/check-compiler-flags.cmake) 
#
# include(CheckCXXCompilerFlag) # 또다른 CMake 기본 모듈을 가져온다
#
# if(MSVC)
#    check_cxx_compiler_flag(/std:c++latest  cxx_latest          )
#    check_cxx_compiler_flag(/W4             high_warning_level  )
# elseif(${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
#    check_cxx_compiler_flag(-std=c++2a      cxx_latest          )
#    check_cxx_compiler_flag(-Wall           high_warning_level  )
# elseif(${CMAKE_CXX_COMPILER_ID} MATCHES GNU)
#    check_cxx_compiler_flag(-std=gnu++2a    cxx_latest          )
#    check_cxx_compiler_flag(-Wextra         high_warning_level  )
# endif()
#

if(cxx_latest)  # include 파일 내에서 설정한 변수를 사용 가능하다
    target_compile_options(...)
endif()

message(STATUS ${CMAKE_SOURCE_DIR})         # -- Root
message(STATUS ${CMAKE_CURRENT_SOURCE_DIR}) # -- Root

add_subdirectory(src)
    # src/CMakeLists.txt를 실행하기 전에 일부 변수들이 새로 설정된다
    #
    #   message(STATUS ${CMAKE_SOURCE_DIR})         # -- Root
    #   message(STATUS ${CMAKE_CURRENT_SOURCE_DIR}) # -- Root/src
    #

add_subdirectory(test)
    #
    #   message(STATUS ${CMAKE_SOURCE_DIR})         # -- Root
    #   message(STATUS ${CMAKE_CURRENT_SOURCE_DIR}) # -- Root/test
    #

# ...
```

#### [`CMAKE_MODULE_PATH`](https://cmake.org/cmake/help/latest/variable/CMAKE_MODULE_PATH.html)

앞서서는 `include`를 위해 아래와 같이 cmake 모듈의 상대 경로를 사용 했었습니다.

```cmake
# ...
include(cmake/check-compiler-flags.cmake) # 경로를 자세하게 제공한 경우
# ...
```

경로를 참조할 때 특정 경로를 참고하도록 지시할 수도 있습니다. 
`CMAKE_MODULE_PATH`를 사용하면, 파일 이름만으로 `include` 하는 것이 가능합니다.
시스템 환경변수 `PATH`와 유사하다고 생각하셨다면 정답입니다.

```cmake
# 현재 프로젝트를 기준으로 cmake 폴더를 CMAKE_MODULE_PATH에 추가한다
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(check-compiler-flags) # 위치한 폴더, .cmake 확장자를 생략해도 문제없다
```

CMake에서 미리 제공하는 모듈들은 https://cmake.org/cmake/help/latest/manual/cmake-modules.7.html 에서 확인할 수 있습니다.

## 설치

### 설치 경로 지정

실행파일은 exe와 dll (혹은 elf와 so)와 같은 binary만 있으면 되지만, 라이브러리는 좀 다릅니다. 엄밀히 말해 빌드된 라이브러리 파일(lib, dll, a, so, dylib ...)과 함께 **링킹을 위한 symbol 정보**가 함께 제공되어야 하기 때문입니다.  
원래라면 exp(export) 파일을 사용하는게 맞겠지만, 개발자의 편의를 생각하면 .h, .hpp 파일을 넘어서기 어려울 것입니다.

#### [`install`](https://cmake.org/cmake/help/latest/command/install.html)

CMake에서는 `install`명령으로 헤더파일, CMake Target, 그리고 필요하다면 폴더를 지정된 위치에 '설치(복사)' 하는 방법을 제공합니다.

```cmake
# 단일 파일을 지정된 폴더에 설치
install(FILE            LICENSE     # ReadMe와 같이 라이브러리와 함께 배포되어야 하는 파일들
        DESTINATION     ./install/
)
# 폴더 전체를 설치
install(DIRECTORY       include     # 헤더 파일들을 통째로 옮긴다
        DESTINATION     ./install/
)
# 빌드 결과물을 설치
install(TARGETS         my_new_library  # add_library, add_executable에 사용했던 이름
        DESTINATION     ./install/
)
```

보통 CMake 프로젝트에서 설치의 대상은 다음 3가지가 있습니다. 
엄밀히 말하면 `.cmake`파일을 설치하는 경우도 있기 때문에 더 많은 종류가 있다고 할 수 있지만, 여러분의 프로젝트가 CMake를 지원하지 않을수도 있으므로 여기서는 설명을 생략하겠습니다.
(오직 빌드만을 위해서 CMake를 사용하는 경우)

 - [프로그램](https://cmake.org/cmake/help/latest/command/install.html#targets)
 - [파일](https://cmake.org/cmake/help/latest/command/install.html#installing-files)
 - [폴더](https://cmake.org/cmake/help/latest/command/install.html#installing-directories)

#### [`CMAKE_INSTALL_PREFIX`](https://cmake.org/cmake/help/latest/variable/CMAKE_INSTALL_PREFIX.html)

하지만 하위 프로젝트들도 제각기 설치 경로를 가지고 있다면 정리하기 어려울 것입니다. 이를 위해 CMake에서는 지정 설치 경로를 의미하는 `CMAKE_INSTALL_PREFIX` 변수가 있습니다.

하위 프로젝트에서 설치 경로를 지정할 때 이 변수를 사용하도록 하면 상위 프로젝트에서 일괄함께 배포하는데 도움을 줄 수 있습니다.

```cmake
# 설치를 CMakeList를 기준으로 하지 않고 CMAKE_INSTALL_PREFIX를 기준으로 수행한다

# 하나의 파일을 옮기는 경우
install(FILE            LICENSE
        DESTINATION     ${CMAKE_INSTALL_PREFIX}/install/
)

# 특정 폴더를 옮기는 경우
install(DIRECTORY       ${CMAKE_CURRENT_SOURCE_DIR}/include
        DESTINATION     ${CMAKE_INSTALL_PREFIX}/install/
)

# add_library, add_executable에 사용한 이름을 설치하는 경우
install(TARGETS         my_new_library
        DESTINATION     ${CMAKE_INSTALL_PREFIX}/install/
)
```

이 변수는 특히 커맨드라인에서 자주 지정하는 변수이기도 합니다. 아래와 같이 설정이 다른 경우 설치 폴더를 분리해서 배포, 경로 참조를 쉽게합니다.

```sh
cmake /path/to/CMakeLists.txt \
    -DCMAKE_INSTALL_PREFIX=~/install/debug/static \    # debug, static
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_SHARED_LIBS=false;
cmake /path/to/CMakeLists.txt \
    -DCMAKE_INSTALL_PREFIX=~/install/debug/dynamic \   # debug, dynamic
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_SHARED_LIBS=true;

cmake /path/to/CMakeLists.txt \
    -DCMAKE_INSTALL_PREFIX=~/install/release/static \  # release, static
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_SHARED_LIBS=false;
cmake /path/to/CMakeLists.txt \
    -DCMAKE_INSTALL_PREFIX=~/install/debug/dynamic \   # release, dynamic
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_SHARED_LIBS=true;
```

위와 같이 실행하고 나면 ~/install 경로에는 아래와 같이 설치될 것입니다.

```console
tree -L 3 ~/install
install
├── LICENSE
├── include
│   └── my_new_lib.h
├── release
│   ├── static
│   │   └── my_new_library.a
│   └── release
│       └── my_new_library.so
└── debug
    ├── static
    │   └── my_new_library.a
    └── release
        └── my_new_library.so
```

빌드 산출물이 여러 종류라면 `install(TARGETS)`에 아래와 같이 `DESTINATION`들을 보다 구체적으로 지정하는 것이 좋습니다.

```c++
install(TARGETS  foo_lib bar_exe 
                 header_only_lib
        INCLUDES DESTINATION ${CMAKE_INSTALL_PREFIX}/include
        RUNTIME  DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
        LIBRARY  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        ARCHIVE  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)
```
