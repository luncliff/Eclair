package dev.luncliff;

import java.lang.System;

public class Module {
    static {
        System.loadLibrary("eclair");
    }

    public static native boolean isTestMode();
}
