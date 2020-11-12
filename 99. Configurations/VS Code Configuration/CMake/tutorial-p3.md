# CMake 튜토리얼 Lv.3

- [Package](#package)
  - [Package의 구성](#package의-구성)
  - [CMake의 Package 찾기](#cmake의-package-찾기)
- [Property](#property)
  - 관련 CMake 함수들
- [CMake Export](#cmake-export)
  - [CMake Manifest 파일의 배치](#cmake-manifest-파일의-배치)
  - [CMake Manifest 만들기](#cmake-manifest-만들기)

## Package

### Package의 구성

보통 패키지라고 하면 [Chocolaty](https://chocolatey.org/), [NuGet](https://docs.microsoft.com/ko-kr/nuget/what-is-nuget), [RPM](https://ko.wikipedia.org/wiki/RPM_%ED%8C%A8%ED%82%A4%EC%A7%80_%EB%A7%A4%EB%8B%88%EC%A0%80), [Brew](https://brew.sh/index_ko)처럼 관리 소프트웨어를 통해 다운로드/설치/업데이트해서 사용하는 프로그램들(+ 문서)을 말하는데, C++ 프로그래머들에게 패키지란 개발에 필요한 Library + Manifest에 가까운 것 같습니다.

* 일반적인 패키지:  
  * 실행 프로그램(executable)
  * 문서 파일(license, manual, readme 등)
* 프로그래밍 패키지: 일반 패키지 + 개발에 필요한 요소들 
  * 서브 프로그램(library)
  * 실행 프로그램(test tools, script 등)
  * 소스 코드(include, example 등)

C++ 에서는 미리 빌드된 서브 프로그램 뿐만 아니라 소스 코드가 포함된다는 점(include)이 특이하다고 할 수 있습니다. 비단 템플릿 프로그래밍의 비중이 늘어난 것 뿐만 아니라 크로스 컴파일과 링킹에 손이 많이 가기 때문이기도 할 것입니다.

지금은 많은 C++ 프로젝트들이 [Unix Filesystem](https://en.wikipedia.org/wiki/Unix_filesystem)에서 표준 C 라이브러리를 배치할때 사용하던 파일트리 구조를 적용하고 있습니다. 
굳이 이런 배치에 어떤 의미가 부여되어있다기 보다는, "CMake의 초창기부터 Unix 시스템에 빌드 된 라이브러리을 설치하면서 관례를 따르던 것이 이어지고 있다"정도로 생각하면 될 것 같습니다.

* bin : 실행 프로그램(executable)
* lib : 미리 빌드된 라이브러리(so, lib 등)
* include : 소스 코드(헤더)
* share : 기타 필요한 파일들. 주로 빌드 지원 파일
   * docs : 문서가 (많이) 있는 경우 따로 두기도

### CMake의 Package 찾기

#### [`find_package`](https://cmake.org/cmake/help/latest/command/find_package.html)

이미 설치된 패키지를 찾는 기능으로 CMake는 `find_package`를 제공하고 있습니다. 
CMake의 패키지를 어떻게 만드는지에 앞서서, 어떻게 사용하는지부터 짚고 넘어가겠습니다. 

잠시 미리 적자면, 여러분이 사용하는 라이브러리가 CMake를 지원하는 경우, `find_package`가 매끄럽게 사용되지 않을 때는 `add_subdirectory`를 사용하는 것이 '정확한' 해결책이 될 수 있습니다. Package export에 문제가 있는 경우 이를 찾아내기에도, **Import하는 쪽에서 수정하기에도 어렵기 때문**입니다.

이 함수는 일반적으로는 아래와 같이 이름과 버전을 인자로 사용합니다. 탐색에 성공하면 `name_FOUND` 변수가 생성됩니다. 
아래 예시처럼 이름으로 `OpenCV`를 사용했다면, 성공여부는 `OpenCV_FOUND`로 확인할 수 있습니다.

```cmake
# optional import
find_package(OpenCV 3.3)
if(OpenCV_FOUND)
    # ...
    # target_source:  Add OpenCV related source codes ...
    # target_compile_options:  Enable RTTI for OpenCV ...
    # ...
endif()

# mandatory import
find_package(OpenCV 3.3 REQUIRED)
```

좀더 상세하게 패키지 탐색을 위한 정보를 제공하는 경우, [`CONFIG`를 사용해 Config Mode로 호출하게 됩니다](https://cmake.org/cmake/help/latest/command/find_package.html#full-signature-and-config-mode).

PATHS를 수정하여도 제대로 찾지 못한다면, CMake Cache의 문제일 가능성이 높습니다.
그런경우 CMakeCache.txt 를 제거하고 다시 CMake를 실행시켜보시기 바랍니다

```cmake
# cmake might find multiple packages. 
# In the case it will peek the first one
find_package(fmt  5.3
CONFIG 
    REQUIRED
    PATHS       C:/vcpkg/installed/x64-windows
                /mnt/vcpkg/installed/x64-linux
)
```

수많은 컴포넌트를 가진 Boost에서 필요한 모듈만 가져다 쓴다면 아래처럼 작성하면 될 것입니다.
분명히 설치 되었음에도 CMake에서 찾지 못한다면 CONFIG를 지우고 다시 시도해보시면 찾을수도 있습니다.

>
> 작성자도 아직 `CONFIG`의 유무가 탐색에 미치는 영향을 명확하게 알아내지는 못했습니다.  
> 대부분의 패키지들은 CONFIG를 함께 쓰면 별 문제없이 탐색에 성공하는 것을 확인했습니다.  
> 아마도 CMake로 export 했는지 여부가 영향을 미치는 것 같다고 짐작할 뿐입니다.
>

```cmake
find_package(Boost  1.59
CONFIG                      # <--- try without CONFIG if the function fails !
    REQUIRED
    COMPONENTS system thread timer
)
```

CMake에서 `find_package`를 호출하면, 해당 함수는 Package를 찾고, 그 안에 있는 Target들을 가져옵니다(`add_library(IMPORTED)`).  
물론 executable과 링킹을 하지는 않기 때문에, 가져온 Target들을 `add_library(INTERFACE)`혹은 `add_library(SHARED)`로 만들어진 결과물들입니다.
따라서 이들을 소비하는 함수는 `target_link_libraries`입니다.

```cmake
find_package(gRPC CONFIG REQUIRED)
# ...
target_link_libraries(main
PRIVATE
    gRPC::gpr gRPC::grpc gRPC::grpc++ gRPC::grpc_cronet
)
```

물론 여기에는 하나의 전제가 있습니다.
해당 라이브러리가 CMake에서 Import할 수 있도록 적절하게 Manifest를 작성해 놓았거나, CMake의 `export`함수를 사용해 CMake를 위한 Manifest를 생성해놓은 것입니다.

>
> Manifest라는 표현은 **이 문서 상에서만** "Package를 내보낸것과 같이 가져오기 위한 목록"이라는 의미로 사용하기에 
> 웹에서 CMake관련 검색할 때 사용하면 오히려 방해가 될 수 있습니다.
>

어떤 파일을 제공해야 하는지 알아보기 위해 아래와 같이 CMakeLists.txt를 작성해 실행해보겠습니다. 

```cmake
cmake_minimum_required(VERSION 3.8)

find_package(TBB REQUIRED)
```

[Intel TBB](https://github.com/intel/tbb)가 설치되지 않은 환경에서 `find_package`가 실패하면서 아래와 같은 오류를 출력할 것입니다.

```console
$ cmake .
...
-- Detecting CXX compile features - done
CMake Error at CMakeLists.txt:3 (find_package):
  By not providing "FindTBB.cmake" in CMAKE_MODULE_PATH this project has
  asked CMake to find a package configuration file provided by "TBB", but
  CMake did not find one.

  Could not find a package configuration file provided by "TBB" with any of
  the following names:

    TBBConfig.cmake
    tbb-config.cmake

  Add the installation prefix of "TBB" to CMAKE_PREFIX_PATH or set "TBB_DIR"
  to a directory containing one of the above files.  If "TBB" provides a
  separate development package or SDK, be sure it has been installed.


-- Configuring incomplete, errors occurred!
```

이를 통해 `find_package`에서 TBB라는 이름을 가지고 대소문자가 혼합된 경우(`TBBConfig.cmake`)와 소문자만 사용된 경우(`tbb-config.cmake`)를 고려하여 Manifest파일을 찾으려 했다는 것을 알 수 있습니다.

#### `-config.cmake`

이전까지는 Manifest파일이라고 하였으나, 이후로는 `-config.cmake`파일이라고 하겠습니다.

TBB를 설치하면 TBBConfig.cmake가 생성된 것을 확인할 수 있습니다. 다행히 TBB의 `-config.cmake`파일은 비교적 짧은 편에 속합니다. Details를 열어 한번 읽어보시기 바랍니다.

<details>
<summary>TBBConfig.cmake <------------------ click me !!!! </summary>

```cmake
# Copyright (c) 2017-2019 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# TBB_FOUND should not be set explicitly. It is defined automatically by CMake.
# Handling of TBB_VERSION is in TBBConfigVersion.cmake.

if (NOT TBB_FIND_COMPONENTS)
    set(TBB_FIND_COMPONENTS "tbb;tbbmalloc;tbbmalloc_proxy")
    foreach (_tbb_component ${TBB_FIND_COMPONENTS})
        set(TBB_FIND_REQUIRED_${_tbb_component} 1)
    endforeach()
endif()

# Add components with internal dependencies: tbbmalloc_proxy -> tbbmalloc
list(FIND TBB_FIND_COMPONENTS tbbmalloc_proxy _tbbmalloc_proxy_ix)
if (NOT _tbbmalloc_proxy_ix EQUAL -1)
    list(FIND TBB_FIND_COMPONENTS tbbmalloc _tbbmalloc_ix)
    if (_tbbmalloc_ix EQUAL -1)
        list(APPEND TBB_FIND_COMPONENTS tbbmalloc)
        set(TBB_FIND_REQUIRED_tbbmalloc ${TBB_FIND_REQUIRED_tbbmalloc_proxy})
    endif()
endif()

set(TBB_INTERFACE_VERSION 11007)

get_filename_component(_tbb_root "${CMAKE_CURRENT_LIST_FILE}" PATH)
get_filename_component(_tbb_root "${_tbb_root}" PATH)
get_filename_component(_tbb_root "${_tbb_root}" PATH)

foreach (_tbb_component ${TBB_FIND_COMPONENTS})
    set(_tbb_release_lib "${_tbb_root}/lib/${_tbb_component}.lib")
    set(_tbb_debug_lib "${_tbb_root}/debug/lib/${_tbb_component}_debug.lib")

    if (EXISTS "${_tbb_release_lib}" OR EXISTS "${_tbb_debug_lib}")
        add_library(TBB::${_tbb_component} UNKNOWN IMPORTED)
        set_target_properties(TBB::${_tbb_component} PROPERTIES
                              INTERFACE_INCLUDE_DIRECTORIES "${_tbb_root}/include")

        if (EXISTS "${_tbb_release_lib}")
            set_target_properties(TBB::${_tbb_component} PROPERTIES
                                  IMPORTED_LOCATION_RELEASE "${_tbb_release_lib}")
            set_property(TARGET TBB::${_tbb_component} APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
        endif()

        if (EXISTS "${_tbb_debug_lib}")
            set_target_properties(TBB::${_tbb_component} PROPERTIES
                                  IMPORTED_LOCATION_DEBUG "${_tbb_debug_lib}")
            set_property(TARGET TBB::${_tbb_component} APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
        endif()

        # Add internal dependencies for imported targets: TBB::tbbmalloc_proxy -> TBB::tbbmalloc
        if (_tbb_component STREQUAL tbbmalloc_proxy)
            set_target_properties(TBB::tbbmalloc_proxy PROPERTIES INTERFACE_LINK_LIBRARIES TBB::tbbmalloc)
        endif()

        list(APPEND TBB_IMPORTED_TARGETS TBB::${_tbb_component})
        set(TBB_${_tbb_component}_FOUND 1)
    elseif (TBB_FIND_REQUIRED AND TBB_FIND_REQUIRED_${_tbb_component})
        message(STATUS "Missed required Intel TBB component: ${_tbb_component}")
        set(TBB_FOUND FALSE)
        set(TBB_${_tbb_component}_FOUND 0)
    endif()
endforeach()

unset(_tbbmalloc_proxy_ix)
unset(_tbbmalloc_ix)
unset(_tbb_lib_path)
unset(_tbb_release_lib)
unset(_tbb_debug_lib)
```
</details>

다소 정리되지 않았다는 느낌이 있지만, 크게 3가지 정도를 눈여겨 볼 수 있습니다.

1. `add_library(IMPORTED)`를 사용해서 CMake Target을 생성합니다. 이름으로는 `TBB::${_tbb_component}`를 사용해서 이것이 CMake Target이라는 점을 분명히 드러내고 있습니다.

2. `set_property`함수를 사용해서 DEBUG/RELEASE 설정으로 빌드되었다는 정보를 추가하는 것을 볼 수 있습니다.

3. `set_target_properties`함수에서 IMPORTED_LOCATION를 사용해 .lib파일의 위치를 지정하거나, INTERFACE_LINK_LIBRARIES를 사용해 `TBB::tbbmalloc_proxy`에서 `TBB::tbbmalloc`를 링킹하도록(의존하도록) 만들고 있습니다.

요약하자면 `find_package`가 하는 일은 `target_link_libraries`에서 적합한 정보(Property)을 받아서 실제 Build System에서 필요로 하는 Linking 정보를 생성할 수 있도록 하는 Target Builder라고 할 수 있겠습니다.

## [Property](https://cmake.org/cmake/help/latest/manual/cmake-properties.7.html)

[CMake에서는 굉장히 많은 Property를 정의하고 있습니다.](https://cmake.org/cmake/help/v3.14/manual/cmake-properties.7.html)
특히 이들을 사용하기 어렵게 만드는 것은, Target의 타입에 따라서 사용할 수 있는 property가 달라진다는 것입니다. 

한때 작성자는 QMake를 볼때처럼 좀 읽다보면 이해하게 되겠거니 하였지만 아주 멍청하고 어리석은 생각이었습니다.
여러분은 직접적으로 Property를 조작하는 일을 멀리하고 참고영상에 나온 용례들을 보시면서 따라하시는게 낫습니다.

#### [`set_property`](https://cmake.org/cmake/help/latest/command/set_property.html)/[`get_property`](https://cmake.org/cmake/help/latest/command/get_property.html)

솔직히 적건대 작성자는 `define_property`, `set_property`, `get_property`를 쓰는 경우는 `-config.cmake`를 제외하고 아직 보지 못했습니다.
여기서는 단순히 함수의 시그니처만 확인할 수 있도록 실행가능한 예시를 적어놓겠습니다.

```cmake
cmake_minimum_required(VERSION 3.8)
add_library(xyz UNKNOWN IMPORTED)

set_property(TARGET xyz APPEND PROPERTY 
    IMPORTED_CONFIGURATIONS RELEASE 
)
get_property(xyz_import_config TARGET xyz PROPERTY
    IMPORTED_CONFIGURATIONS
)

message(STATUS ${xyz_import_config})
```

#### [`set_target_properties`](https://cmake.org/cmake/help/latest/command/set_target_properties.html)

3.x 버전의 CMake에서 export 된 `-config.cmake`파일들은 대부분 아래와 같은 Property들을 설정합니다.

* `INTERFACE_INCLUDE_DIRECTORIES`: 헤더 파일이 위치한 폴더들  
   `/usr/local/include;/usr/include` 형태로 ';'을 써서 여러 폴더를 지정할 수 있습니다.

* `INTERFACE_LINK_LIBRARIES`: 현재 Target의 의존성을 보여주는 부분입니다.  
   `target_link_libraries`에서 필요로 하는 인자, 즉 다른 CMake Target들의 이름을 ';'로 구분되는 목록을 사용해서 지정합니다.  
   (`INTERFACE_INCLUDE_DIRECTORIES`와 동일)

* `IMPORTED_LOCATION`: 서브 프로그램의 위치를 '절대경로'로 지정합니다.  
    지금까지는 대부분 상대경로로 해결할 수 있었으나, 여기서 절대경로만을 허용하는 이유는 지금 `find_package`하는 대상이 이미 **설치**되었기 때문일 것입니다.

* `IMPORTED_IMPLIB`: Windows의 경우 링킹을 위해 .lib파일이 필요하기도 합니다.  
    다른 플랫폼에서는 사용되는 것을 보지 못했습니다.

실제 사용하는 모습은 다음과 같습니다.

```cmake
add_library(xyz UNKNOWN IMPORTED)

set_target_properties(xyz
PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES   ${INTERFACE_DIR}
    INTERFACE_LINK_LIBRARIES        "OpenMP::OpenMP_CXX"
)

set_target_properties(xyz
PROPERTIES
    IMPORTED_LOCATION   ${LIBS_DIR}/iphone/libxyz.a
)
set_target_properties(xyz
PROPERTIES
    IMPORTED_IMPLIB     ${LIBS_DIR}/windows/xyz.lib
    IMPORTED_LOCATION   ${LIBS_DIR}/windows/xyz.dll
)
```

덧붙여, Build Target을 작성할때 작성자는 언제나 `CXX_STANDARD`를 명시합니다. 이는 `target_compile_options`함수로 `/std:c++latest`혹은 `gnu++2a`를 추가하지 않아도 자동으로 추가하도록 해줍니다. 이 Property의 최대 값은 CMake 버전에 따라서 결정됩니다.

```cmake
cmake_minimum_required(VERSION 3.8)

add_library(my_modern_cpp_lib
    src/libmain.cpp
)

set_target_properties(my_modern_cpp_lib
PROPERTIES
    CXX_STANDARD 17
)
```

작성자가 지금까지 항상 3.8 버전을 사용한 이유가 여기에 있습니다. CMake 3.14부터는 C++ 20을 명시할 수 있습니다.

```cmake
cmake_minimum_required(VERSION 3.14)

add_library(my_modern_cpp_lib
    src/libmain.cpp
)

set_target_properties(my_modern_cpp_lib
PROPERTIES
    CXX_STANDARD 20
)
```


#### [`CMAKE_CURRENT_LIST_FILE`](https://cmake.org/cmake/help/latest/variable/CMAKE_CURRENT_LIST_FILE.html)

절대 경로를 지정해야 하는 경우, `/usr/local`과 같이 잘 알려진 경로면 좋겠지만 그렇지 못한 경우 해당 `-config.cmake`를 기준으로 탐색을 해야 할수도 있습니다. 여기에는 보통 `CMAKE_CURRENT_LIST_FILE` 변수가 사용됩니다.  
이 변수는 `include`되는 `.cmake` 파일의 위치를 저장하고 있습니다. 
물론 `CMakeLists.txt`도 예외가 아닙니다.

아래와 같이 파일이 배치되었다고 가정해보겠습니다.
```console
$ tree $(pwd)
/path/to
├── CMakeLists.txt
└── cmake
    ├── print-current-path.cmake
    └── print-parent-path.cmake

1 directory, 3 files
```

각각의 내용이 아래와 같다면...

```cmake
# cmake/print-current-path.cmake
message(STATUS "cmake    : ${CMAKE_CURRENT_LIST_FILE}")

# CMakeLists.txt
cmake_minimum_required(VERSION 3.8)

include(cmake/print-current-path.cmake)
message(STATUS "cmakelist: ${CMAKE_CURRENT_LIST_FILE}")
```

이런 결과가 출력될 것입니다.

```console
$ cmake .
...
-- cmake    : /path/to/cmake/print-filepath.cmake
-- cmakelist: /path/to/CMakeLists.txt
...
-- Configuring done
-- Generating done
```

#### [`get_filename_component`](https://cmake.org/cmake/help/latest/command/get_filename_component.html)

보통 특정 경로 하나만으로는 문제를 해결할 수 없기 때문에 여기서는 경로를 다루는 방법 중 두가지를 짚고 넘어가겠습니다. 

기본적으로 CMake에서 파일의 경로 생성할때는 `get_filename_component`를 사용합니다.
앞서 `TBBConfig.cmake`에서도 이 함수가 사용되었었는데, 코드를 보면 의도를 파악하기가 어렵습니다.

```cmake
# ...
get_filename_component(_tbb_root "${CMAKE_CURRENT_LIST_FILE}" PATH)
get_filename_component(_tbb_root "${_tbb_root}" PATH)
get_filename_component(_tbb_root "${_tbb_root}" PATH)
# ...
```

##### Parent Path (Removing Base Name)

CMake 문서에 따르면 가장 마지막에 사용된 인자 `PATH`는 2.8 버전들의 하위호환을 위한 것으로, 그 의미는 `DIRECTORY`를 사용하는 것과 동일합니다.

```
DIRECTORY = Directory without file name
PATH      = Legacy alias for DIRECTORY (use for CMake <= 2.8.11)
```

따라서 현재 기술하고 있는 3.8 이후 버전을 기준으로 작성한다면 아래와 같을 것입니다.

```cmake
# ...
get_filename_component(_tbb_root "${CMAKE_CURRENT_LIST_FILE}" DIRECTORY)
get_filename_component(_tbb_root "${_tbb_root}" DIRECTORY)
get_filename_component(_tbb_root "${_tbb_root}" DIRECTORY)
# ...
```

이미 설계된 TBB 빌드 결과물의 배치를 고려해서 부모 폴더를 여러번 타고 올라가는 코드라는 것을 쉽게 알 수 있습니다. 이를 `CMAKE_CURRENT_LIST_FILE`에 적용해보면 어떻게 될까요?

```cmake
# cmake/print-parent-path.cmake

get_filename_component(PARENT_DIR ${CMAKE_CURRENT_LIST_FILE} DIRECTORY)
message(STATUS "parent   : ${PARENT_DIR}")
```

조금 전에 `CMAKE_CURRENT_LIST_FILE`에서 사용한 CMakeLists.txt를 아래처럼 수정해 실행하면:

```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.8)

include(cmake/print-current-path.cmake)
include(cmake/print-parent-path.cmake)  # <--- new !
message(STATUS "cmakelist: ${CMAKE_CURRENT_LIST_FILE}")
```

출력 결과는 아래와 같을 것입니다.

```console
$ cmake .
-- cmake    : /path/to/cmake/print-current-path.cmake
-- parent   : /path/to/cmake
-- cmakelist: /path/to/CMakeLists.txt
...
-- Configuring done
-- Generating done
```

##### Path Join

경로를 처리할때 접합(concat)을 수행하는 코드를 흔히 볼 수 있습니다.
이런 코드들은 절대 경로(Absolute Path)와 상대 경로(Relative Path)가 고르게 사용되는 반면, 
CMake에서 파일 경로는 특별한 처리가 필요하지 않는 한 절대 경로를 사용합니다. 

> 
> 작성자의 생각으로는, `PROJECT_SOURCE_DIR`, `CMAKE_CURRENT_SOURCE_DIR` 등 파일경로를 만들때 가장 기초가 되는 경로가 모두 절대경로로 반환되기 때문인 것 같습니다.
>

이미 존재하는 **폴더** 경로에 새로운 이름을 붙이는 것은 보통의 문자열 생성 방법과 같습니다. Windows에서는 Command Prompt를 실행하는 경우라면 `\\`를 구분자로 사용해야 하지만, 단순히 CMake 내에서 경로만 처리한다면 `/`를 사용해도 별다른 문제가 없습니다.

```cmake
# Ok for Windows and the others
get_filename_component(CURRENT_MODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake ABSOLUTE)
message(STATUS "modules  : ${CURRENT_MODULE_DIR}")
```

여기서 `get_filename_component`의 역할은 `CURRENT_MODULE_DIR`변수의 타입을 파일경로로 설정하는 것 뿐입니다. Windows, PowerShell 환경에서 이를 실행해보면 CMake에서 구분자로 `/`를 사용하는 것을 확인할 수 있습니다.

```console
PS > cmake .
...
-- modules  : D:/path/to/cmake
...
```

WSL Bash에서는 아래와 같습니다.

```console
$ cmake .
...
-- modules  : /mnt/d/cmake_tutorial/path/cmake
...
```



#### [`get_target_property`](https://cmake.org/cmake/help/latest/command/get_target_property.html)

`set_target_properties`가 여러 Property를 한번에 설정할 수 있는데 반해, `get_target_property`는 한번에 하나의 변수를 생성합니다. 사용법 또한 굉장히 단순합니다.

```cmake
cmake_minimum_required(VERSION 3.8)

add_library(my_modern_cpp_lib
    libmain.cpp
)

set_target_properties(my_modern_cpp_lib
PROPERTIES
    CXX_STANDARD 17
)
get_target_property(specified_cxx_version
                    my_modern_cpp_lib CXX_STANDARD
)

# -- cxx_version: 17
message(STATUS "cxx_version: ${specified_cxx_version}")
```

이제 `find_package`는 Target을 선언하고 Property들을 설정한다는 것과 Property를 설정하고 확인하는 함수들을 다루었으므로
빌드 이후 Manifest파일을 생성하는 방법에 대해 다뤄보겠습니다.

## [CMake Export](https://cmake.org/cmake/help/latest/command/export.html)

사실 CMake에서 Export하는 방법은 튜토리얼마다 설명이 조금씩 다른데, 근본적인 차이점은 CMake를 위한 템플릿 파일을 사용하는지에 달려 있습니다.
어떤 프로젝트에서는 CMake 모듈들이 배치된 폴더에 `package-targets.cmake.in`과 같에 `.in`으로 끝나는 파일들이 있는 것을 볼 수 있는데, 
이런 인라인 파일들은 어디선가 CMake에서 제공하는 [`configure_file`](https://cmake.org/cmake/help/latest/command/configure_file.html) 혹은 [`configure_package_config_file`](https://cmake.org/cmake/help/latest/module/CMakePackageConfigHelpers.html#generating-a-package-version-file)함수를 사용하기 때문일 가능성이 높습니다.

이 함수는 CMake파일 생성 뿐만 아니라 사용자 환경에 맞는 헤더 파일(.h)을 만들거나, Linux플랫폼에서 에서 pkg-config를 위한 파일을 만드는데 사용되기도 합니다.

>
> 작성자는 이 기능을 사용하는 것을 추천하지 않습니다.  
> 빌드 시스템 파일을 생성하는것이 CMake의 가장 중요한 부분이며, 오직 그 일에 집중해야 한다고 생각하기 때문입니다
>

### CMake Manifest 파일의 배치

지금까지 `find_package`에 어떤 인자를 사용하는지, 해당 함수에서 사용하는 Manifest 파일에 어떤 내용이 들어가는지는 살펴보았으나, **어디에서** 해당 파일을 찾는지는 설명하지 않았습니다.

#### [CMake의 Manifest 탐색 과정](https://cmake.org/cmake/help/latest/command/find_package.html#search-procedure)

[CMake 문서의 설명](https://cmake.org/cmake/help/latest/command/find_package.html#search-procedure)에 따르면 플랫폼마다 탐색 경로가 다르지만, 공통되는 경로가 있다는 것을 알 수 있습니다.
앞서 이 문서에서는 `install`을 사용할 때 `CMAKE_INSTALL_PREFIX`를 기준으로 설치경로를 지정하는 것을 권했었는데,
아마 아래처럼 경로에 프로젝트 이름이 들어가는 것이 다른 프로젝트와의 충돌의 가능성을 낮춰줄 것입니다.

```cmake
cmake_minimum_required(VERSION 3.8)
project(my_modern_cpp_lib LANGUAGES CXX)
# ...

install(FILES           ${VERSION_FILE_PATH} 
                        ${LICENSE_FILE_PATH}
        DESTINATION     ${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}
)
```

### CMake Manifest 만들기

#### [`write_basic_package_version_file`](https://cmake.org/cmake/help/latest/module/CMakePackageConfigHelpers.html#generating-a-package-version-file)

버전 정보를 추가하는 것은 이미 [CMake에서 제공하는 모듈](https://cmake.org/cmake/help/latest/module/CMakePackageConfigHelpers.html#cmakepackageconfighelpers)을 사용하면 쉽게 작성할 수 있습니다.

```cmake
include(CMakePackageConfigHelpers)
set(VERSION_FILE_PATH   ${CMAKE_BINARY_DIR}/cmake/${PROJECT_NAME}-config-version.cmake)
write_basic_package_version_file(${VERSION_FILE_PATH}
    VERSION             ${PROJECT_VERSION} # x.y.z
    COMPATIBILITY       SameMajorVersion
)

# ...

install(FILES           ${VERSION_FILE_PATH} 
        DESTINATION     ${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}
)
```

#### [`install(EXPORT)`](https://cmake.org/cmake/help/latest/command/install.html#export)

[파일](https://cmake.org/cmake/help/latest/command/install.html#installing-files) 혹은 
[폴더](https://cmake.org/cmake/help/latest/command/install.html#installing-directories)의 설치는 단순히 복사/갱신으로 끝날 수 있지만,
결정적으로 `-config.cmake`에는 프로젝트에서 빌드할 [Target](https://cmake.org/cmake/help/latest/command/install.html#targets)에 대한 정보가 들어가야 합니다.
여기에는 `install(TARGETS)`와 `install(EXPORT)`가 함께 사용됩니다.


간단한 예시로, 아래와 같은 구조의 프로젝트를 만들어보겠습니다.

```console
$ tree $(pwd)
/mnt/d/example
├── CMakeLists.txt
└── src
    ├── CMakeLists.txt
    └── libmain.cpp

1 directory, 3 files
```

우선 Root CMakeLists.txt에서는 `EXPORT_NAME`변수를 만들고 `add_subdirectory`로 하위 모듈들을 빌드하도록 합니다. 최종적으로는 `install(EXPORT)`를 사용해 설치까지 수행합니다.

```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.8)
project(stones LANGUAGES CXX)

set(EXPORT_NAME ${PROJECT_NAME}-config) # or ${PROJECT_NAME}Config

add_subdirectory(src) # <--- uses EXPORT_NAME

install(EXPORT      ${EXPORT_NAME}
        NAMESPACE   stones::
        DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}
)
```

src의 CMakeLists.txt는 `add_library`로 CMake Target을 생성하고, `install(TARGETS)`에서 `EXPORT` 인자를 사용해 해당 라이브러리를 일종의 Export Group에 추가합니다.
단순히 추가하기만 할 뿐, `install(EXPORT)`를 사용하기 전까지 실제 설치는 이루어지지 않습니다.  
**특이하게도 `EXPORT`는 반드시 다른 인자보다 먼저 사용되어야 한다고 명시하고 있습니다(must appear before).**

```cmake
# src/CMakeLists.txt

add_library(stone1 SHARED
    libmain.cpp
)
set_target_properties(stone1
PROPERTIES
    CXX_STANDARD 17
)
target_include_directories(stone1
PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>
)

install(TARGETS         stone1
        EXPORT          ${EXPORT_NAME}  # <---- new!
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)
```

마지막으로 libmain.cpp는 간단히 함수 하나를 동적 링킹(Dynamic Linking)이 가능하도록 정의합니다.

```c++
#pragma once
// clang-format off
#if defined(_MSC_VER) // MSVC or clang-cl
#   define _HIDDEN_
#   ifdef _WINDLL
#       define _INTERFACE_ __declspec(dllexport)
#   else
#       define _INTERFACE_ __declspec(dllimport)
#   endif
#elif defined(__GNUC__) || defined(__clang__)
#   define _INTERFACE_ __attribute__((visibility("default")))
#   define _HIDDEN_ __attribute__((visibility("hidden")))
#else
#   error "unexpected linking configuration"
#endif
// clang-format on

#include <cstdint>

constexpr auto version_code = 0x0102;

_INTERFACE_ uint32_t get_version() noexcept;

uint32_t get_version() noexcept{
    return version_code;
}
```

#### Export 결과 확인

Windows에서 설치를 수행하면 아래와 같이 `stones-config.cmake`파일이 설치되는 것을 볼 수 있습니다.

```console
PS D:\examples\build> cmake --build . --config debug --target install 
PS D:\install> Tree /f .
Folder PATH listing for volume keep
Volume serial number is B47E-DE87
D:\INSTALL
├─bin
│      stone1.dll
│
├─lib
│      stone1.lib
│
└─share
    └─stones
            stones-config-debug.cmake
            stones-config.cmake

```

여기서 설치된 파일의 이름인 `stones-config`는 앞서 `EXPORT_NAME` 변수의 값을 따른 것입니다.

```cmake
set(EXPORT_NAME ${PROJECT_NAME}-config) # or ${PROJECT_NAME}Config
```

불필요한 부분을 제외하고 해당 파일의 내용을 살펴보면 `stones::stone1`와 같이 Target을 가져오는 내용이라는 것을 알 수 있습니다.
이런 파일들은 `stones-targets.cmake`로 따로 만들고 `-config.cmake`는 `configure_package_config_file`을 사용해서 만드는 방법을 사용하기도 합니다.
하지만 이 예시에서는 Import측에 전달할 정보가 없기에 CMake 템플릿 파일을 작성하지 않았고, 따라서 바로 `-config.cmake`를 생성해도 무방합니다.

```cmake
# stones-config.cmake
# ...

# The installation prefix configured by this project.
set(_IMPORT_PREFIX "D:/install")

# Create imported target stones::stone1
add_library(stones::stone1 SHARED IMPORTED)

set_target_properties(stones::stone1 PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "D:/install/include"
)

# Load information for each installed configuration.
get_filename_component(_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
file(GLOB CONFIG_FILES "${_DIR}/stones-config-*.cmake")
foreach(f ${CONFIG_FILES})
  include(${f})
endforeach()

# ...
```

특히 패턴매칭(`stones-config-*.cmake`)을 사용해 `-config-debug.cmake`혹은 `-config-release.cmake`를 `include`할 수 있도록 되어있는 점에 주목하시길 바랍니다.
앞서 `write_basic_package_version_file`에서 Version 파일의 설치 위치를 비롯해 이름을 `${PROJECT_NAME}-config-version.cmake`로 만들도록 한 것은 이를 고려한 것입니다.

```cmake
set(EXPORT_NAME ${PROJECT_NAME}-config)

add_subdirectory(src) # <--- uses EXPORT_NAME

install(EXPORT          ${EXPORT_NAME}
        NAMESPACE       stones::
        DESTINATION     ${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}
)

include(CMakePackageConfigHelpers)
set(VERSION_FILE_PATH   ${CMAKE_BINARY_DIR}/cmake/${PROJECT_NAME}-config-version.cmake)
write_basic_package_version_file(${VERSION_FILE_PATH}
    VERSION             ${PROJECT_VERSION} # x.y.z
    COMPATIBILITY       SameMajorVersion
)
install(FILES           ${VERSION_FILE_PATH} 
        DESTINATION     ${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}
)
```

#### `target_include_directories`: Build >> Install

좀 전의 예시에서 처음으로 보인 `BUILD_INTERFACE`와 `INSTALL_INTERFACE`의 사용을 한마디로 정리하자면,  
"빌드할 때 사용하는 include 폴더와 설치 후 사용하는 include 폴더가 다르다" 라는 것입니다.

```
target_include_directories(stone1
PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>
)
```

위와 같이 작성하는 것을 CMake에서는 [Generator Expression](https://cmake.org/cmake/help/latest/manual/cmake-generator-expressions.7.html)이라 하는데, 보통 플랫폼에 따라 IF/ELSE/AND/OR이 뒤섞여 가독성을 심하게 해치는 경향이 있습니다.

라이브러리가 설치된 이후에는 Build에 사용한 폴더가 삭제될 가능성이 높기에, Import할 때 소스코드가 배치된 폴더를 사용하도록 한다면 파일을 못찾는 문제가 발생할 것입니다.
이를 막기 위해 빌드시에는 `PROJECT_SOURCE_DIR`기준으로 include를 수행하지만, 설치 이후에는 `CMAKE_INSTALL_PREFIX`를 기준으로 include를 수행합니다.

아마 인터페이스 파일들은 이미 `CMAKE_INSTALL_PREFIX/include`로 `install(FILES)` 혹은 `install(DIRECTORIES)`를 통해서 복사되었을 것이기에 설치가 왼료된 시점부터 해당 폴더는 사용가능한 경로가 될 것입니다.

