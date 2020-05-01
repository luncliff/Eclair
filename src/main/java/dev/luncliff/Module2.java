package dev.luncliff;

import java.lang.System;

public class Module2 {
    static {
        System.loadLibrary("eclair");
    }

    public static native boolean isTestMode();
}
