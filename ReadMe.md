# Eclair

[![Build Status](https://dev.azure.com/luncliff/personal/_apis/build/status/luncliff.Eclair?branchName=master)](https://dev.azure.com/luncliff/personal/_build/latest?definitionId=38&branchName=master) [![](https://img.shields.io/badge/Gradle-6.0%2B-%2302303A)](https://gradle.org)

[Gradle](https://gradle.org/features/)만으로 JNI 모듈을 빌드/테스트/패키징 하는 템플릿 프로젝트

### References

* 책
    * Gradle 철저 입문 - 김완섭 옮김, 길벗 출판사
    * 안드로이드의 모든 것 NDK - 고현철,전호철 지음, 한빛미디어
* Gradle 공식 문서, 예제
    * **[Introducing the new C++ plugins](https://blog.gradle.org/introducing-the-new-cpp-plugins)**
    * [Gradle Build Language Reference](https://docs.gradle.org/6.0.1/dsl/index.html)
    * [Building C++ Projects](https://docs.gradle.org/6.0.1/userguide/building_cpp_projects.html)
    * [Building and Testing C++ Libraries](https://guides.gradle.org/building-cpp-libraries/)
    * [GitHub: Gradle Native Project Samples](https://github.com/gradle/native-samples)
* Apache Maven
    * [Introduction to the Standard Directory Layout](http://maven.apache.org/guides/introduction/introduction-to-the-standard-directory-layout.html)
* Java Native Interface(JNI)
    * [Specification](https://docs.oracle.com/en/java/javase/11/docs/specs/jni/index.html)
* JUnit5
    * [JUnit 5 User Guide](https://junit.org/junit5/docs/current/user-guide/)
    * [IBM Developer Tutorial: The JUnit 5 Jupiter API](https://developer.ibm.com/tutorials/j-introducing-junit5-part1-jupiter-api/)

### Code Organization

기본적으로 Gradle로 빌드되는 Sub-project로 가정하며, [Apache Maven의 구성 방법](http://maven.apache.org/guides/introduction/introduction-to-the-standard-directory-layout.html)을 기본으로 하되,  
각 단계를 진행하며 달라진 부분을 눈치채고 변경할 수 있도록 구성하였습니다.

## How To

Java, Gradle을 설치한 후, [docs](./docs)에서 단계적으로 따라할 수 있도록 작성되었습니다.

### Setup

* JDK 11+
* Gradle 6.0+

#### Windows

윈도우 환경에서는 [Chocolatey](https://chocolatey.org/)를 사용

```ps1
choco install openjdk
choco install gradle
```

설치 이후 Gradle에서 요구하는 환경변수 `JAVA_HOME` 및 일반적으로 Java 툴킷에 필요한 `JDK_HOME`을 설정. (빌드에서는 `JAVA_HOME`만을 사용)

```ps1
Write-Output $env:JAVA_HOME
# C:\Program Files\OpenJDK\jdk-13.0.1
Write-Output $env:JDK_HOME
# C:\Program Files\OpenJDK\jdk-13.0.1
```

#### UNIX-like

[SDKMan](https://sdkman.io/)을 사용. 아래의 내용은 프로젝트 목적을 고려해 [설치 과정](https://sdkman.io/install)을 요약한 것.

```console
user@host:~$ sudo apt install -y -qq unzip zip curl
user@host:~$ curl -s "https://get.sdkman.io" | bash
user@host:~$ source "/home/luncliff/.sdkman/bin/sdkman-init.sh"
```

Console 재시작.

```console
user@host:~$ sdk version
...
SDKMAN 5.7.4+362
```

JDK 설치. 설치 후, `JDK_HOME`, `JAVA_HOME` 환경변수 설정필요. 두 값은 같은 값을 사용하며, 모두 `javac` 프로그램의 위치를 사용해 획득 가능.

```console
user@host:~$ sdk install java 13.0.1-open
...
user@host:~$ javac --version
javac 13.0.1
user@host:~$ export JDK_HOME="$(dirname $(which javac))"
user@host:~$ export JAVA_HOME=${JDK_HOME}
```

이후 Gradle 설치 및 버전 확인

```console
user@host:~$ sdk install gradle 6.0.1
...
user@host:~$ gradle --version

------------------------------------------------------------
Gradle 6.0.1
------------------------------------------------------------

Build time:   2019-11-18 20:25:01 UTC
Revision:     fad121066a68c4701acd362daf4287a7c309a0f5
...
```
