package com.ryugraph;

/**
 * Version is a class to get the version of the Ryu.
 */
public class Version {

    /**
     * Get the version of the Ryu.
     *
     * @return The version of the Ryu.
     */
    public static String getVersion() {
        return Native.ryuGetVersion();
    }

    /**
     * Get the storage version of the Ryu.
     *
     * @return The storage version of the Ryu.
     */
    public static long getStorageVersion() {
        return Native.ryuGetStorageVersion();
    }
}
