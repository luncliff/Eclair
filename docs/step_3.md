# .zip 으로 패키징하기

> (WIP)

### Packaging

개발 과정에서는 build 명령으로 충분하지만, [패키징에는 `assembleRelease`가 포함되어야 하므로](https://docs.gradle.org/6.0.1/userguide/cpp_library_plugin.html#sec:cpp_library_tasks) 다음과 같이 추가 빌드 이후 `distZip`을 사용해 zip 파일 생성.

```bash
gradle -b cpp.gradle  assembleDebug assembleRelease
gradle -b build.gradle build distZip
```

생성 결과는 'build/distributions'에 위치.

```console
PS C:\Users\luncl\eclair> dir .\build\distributions

    Directory: C:\Users\luncl\eclair\build\distributions

Mode                LastWriteTime         Length Name
----                -------------         ------ ----
-a----       11/24/2019   5:29 PM        5232640 eclair-0.4.tar
-a----       11/24/2019   6:09 PM        8553158 eclair-0.4.zip
```
