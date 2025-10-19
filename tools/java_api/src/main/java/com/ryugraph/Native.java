package com.ryugraph;

import java.util.Map;
import java.io.File;
import java.io.IOException;
import java.net.URL;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardCopyOption;

/**
 * Native is a wrapper class for the native library.
 * It is used to load the native library and call the native functions.
 * This class is not intended to be used by end users.
 */
public class Native {
    static {
        try {
            String os_name = "";
            String os_arch;
            String os_name_detect = System.getProperty("os.name").toLowerCase().trim();
            String os_arch_detect = System.getProperty("os.arch").toLowerCase().trim();
            boolean isAndroid = System.getProperty("java.runtime.name", "").toLowerCase().contains("android")
                || System.getProperty("java.vendor", "").toLowerCase().contains("android")
                || System.getProperty("java.vm.name", "").toLowerCase().contains("dalvik");
            switch (os_arch_detect) {
                case "x86_64":
                case "amd64":
                    os_arch = "amd64";
                    break;
                case "aarch64":
                case "arm64":
                    os_arch = "arm64";
                    break;
                case "i386":
                    os_arch = "i386";
                    break;
                default:
                    throw new IllegalStateException("Unsupported system architecture");
            }
            if (isAndroid){
                os_name = "android";
            }
            else if (os_name_detect.startsWith("windows")) {
                os_name = "windows";
            } else if (os_name_detect.startsWith("mac")) {
                os_name = "osx";
            } else if (os_name_detect.startsWith("linux")) {
                os_name = "linux";
            }
            String lib_res_name = "/libryu_java_native.so" + "_" + os_name + "_" + os_arch;

            Path lib_file = Files.createTempFile("libryu_java_native", ".so");
            URL lib_res = Native.class.getResource(lib_res_name);
            if (lib_res == null) {
                throw new IOException(lib_res_name + " not found");
            }
            Files.copy(lib_res.openStream(), lib_file, StandardCopyOption.REPLACE_EXISTING);
            new File(lib_file.toString()).deleteOnExit();
            String lib_path = lib_file.toAbsolutePath().toString();
            System.load(lib_path);
            if (os_name.equals("linux")) {
                ryuNativeReloadLibrary(lib_path);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    // Hack: Reload the native library again in JNI bindings to work around the
    // extension loading issue on Linux as System.load() does not set
    // `RTLD_GLOBAL` flag and there is no way to set it in Java.
    protected static native void ryuNativeReloadLibrary(String libPath);

    // Database
    protected static native long ryuDatabaseInit(String databasePath, long bufferPoolSize,
            boolean enableCompression, boolean readOnly, long maxDbSize, boolean autoCheckpoint,
            long checkpointThreshold,boolean throwOnWalReplayFailure, boolean enableChecksums);

    protected static native void ryuDatabaseDestroy(Database db);

    protected static native void ryuDatabaseSetLoggingLevel(String loggingLevel);

    // Connection
    protected static native long ryuConnectionInit(Database database);

    protected static native void ryuConnectionDestroy(Connection connection);

    protected static native void ryuConnectionSetMaxNumThreadForExec(
            Connection connection, long numThreads);

    protected static native long ryuConnectionGetMaxNumThreadForExec(Connection connection);

    protected static native QueryResult ryuConnectionQuery(Connection connection, String query);

    protected static native PreparedStatement ryuConnectionPrepare(
            Connection connection, String query);

    protected static native QueryResult ryuConnectionExecute(
            Connection connection, PreparedStatement preparedStatement, Map<String, Value> param);

    protected static native void ryuConnectionInterrupt(Connection connection);

    protected static native void ryuConnectionSetQueryTimeout(
            Connection connection, long timeoutInMs);

    // PreparedStatement
    protected static native void ryuPreparedStatementDestroy(PreparedStatement preparedStatement);

    protected static native boolean ryuPreparedStatementIsSuccess(PreparedStatement preparedStatement);

    protected static native String ryuPreparedStatementGetErrorMessage(
            PreparedStatement preparedStatement);

    // QueryResult
    protected static native void ryuQueryResultDestroy(QueryResult queryResult);

    protected static native boolean ryuQueryResultIsSuccess(QueryResult queryResult);

    protected static native String ryuQueryResultGetErrorMessage(QueryResult queryResult);

    protected static native long ryuQueryResultGetNumColumns(QueryResult queryResult);

    protected static native String ryuQueryResultGetColumnName(QueryResult queryResult, long index);

    protected static native DataType ryuQueryResultGetColumnDataType(
            QueryResult queryResult, long index);

    protected static native long ryuQueryResultGetNumTuples(QueryResult queryResult);

    protected static native QuerySummary ryuQueryResultGetQuerySummary(QueryResult queryResult);

    protected static native boolean ryuQueryResultHasNext(QueryResult queryResult);

    protected static native FlatTuple ryuQueryResultGetNext(QueryResult queryResult);

    protected static native boolean ryuQueryResultHasNextQueryResult(QueryResult queryResult);

    protected static native QueryResult ryuQueryResultGetNextQueryResult(QueryResult queryResult);

    protected static native String ryuQueryResultToString(QueryResult queryResult);

    protected static native void ryuQueryResultResetIterator(QueryResult queryResult);

    // FlatTuple
    protected static native void ryuFlatTupleDestroy(FlatTuple flatTuple);

    protected static native Value ryuFlatTupleGetValue(FlatTuple flatTuple, long index);

    protected static native String ryuFlatTupleToString(FlatTuple flatTuple);

    // DataType
    protected static native long ryuDataTypeCreate(
            DataTypeID id, DataType childType, long numElementsInArray);

    protected static native DataType ryuDataTypeClone(DataType dataType);

    protected static native void ryuDataTypeDestroy(DataType dataType);

    protected static native boolean ryuDataTypeEquals(DataType dataType1, DataType dataType2);

    protected static native DataTypeID ryuDataTypeGetId(DataType dataType);

    protected static native DataType ryuDataTypeGetChildType(DataType dataType);

    protected static native long ryuDataTypeGetNumElementsInArray(DataType dataType);

    // Value
    protected static native Value ryuValueCreateNull();

    protected static native Value ryuValueCreateNullWithDataType(DataType dataType);

    protected static native boolean ryuValueIsNull(Value value);

    protected static native void ryuValueSetNull(Value value, boolean isNull);

    protected static native Value ryuValueCreateDefault(DataType dataType);

    protected static native <T> long ryuValueCreateValue(T val);

    protected static native Value ryuValueClone(Value value);

    protected static native void ryuValueCopy(Value value, Value other);

    protected static native void ryuValueDestroy(Value value);

    protected static native Value ryuCreateMap(Value[] keys, Value[] values);

    protected static native Value ryuCreateList(Value[] values);

    protected static native Value ryuCreateList(DataType type, long numElements);

    protected static native long ryuValueGetListSize(Value value);

    protected static native Value ryuValueGetListElement(Value value, long index);

    protected static native DataType ryuValueGetDataType(Value value);

    protected static native <T> T ryuValueGetValue(Value value);

    protected static native String ryuValueToString(Value value);

    protected static native InternalID ryuNodeValGetId(Value nodeVal);

    protected static native String ryuNodeValGetLabelName(Value nodeVal);

    protected static native long ryuNodeValGetPropertySize(Value nodeVal);

    protected static native String ryuNodeValGetPropertyNameAt(Value nodeVal, long index);

    protected static native Value ryuNodeValGetPropertyValueAt(Value nodeVal, long index);

    protected static native String ryuNodeValToString(Value nodeVal);

    protected static native InternalID ryuRelValGetId(Value relVal);

    protected static native InternalID ryuRelValGetSrcId(Value relVal);

    protected static native InternalID ryuRelValGetDstId(Value relVal);

    protected static native String ryuRelValGetLabelName(Value relVal);

    protected static native long ryuRelValGetPropertySize(Value relVal);

    protected static native String ryuRelValGetPropertyNameAt(Value relVal, long index);

    protected static native Value ryuRelValGetPropertyValueAt(Value relVal, long index);

    protected static native String ryuRelValToString(Value relVal);

    protected static native Value ryuCreateStruct(String[] fieldNames, Value[] fieldValues);

    protected static native String ryuValueGetStructFieldName(Value structVal, long index);

    protected static native long ryuValueGetStructIndex(Value structVal, String fieldName);

    protected static native String ryuGetVersion();

    protected static native long ryuGetStorageVersion();
}
