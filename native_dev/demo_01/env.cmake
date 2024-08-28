set(QT_ROOT C:/Qt/6.7.0/msvc2019_64)
#安装包需要的路径参数, 需要设置为本机实际路径
set(YK_7Z_PATH "F:/Program Files/7-Zip")
set(YK_NSIS_PATH "F:/newNSIS3.06.1") 

set(LIBWEBRTC_INCLUDE_PATH C:/code/hub/webrtc3/src)
set(LIBWEBRTC_BINARY_PATH C:/code/hub/webrtc3/src/out/m120_release/obj)

set(ENV{http_proxy} "http://127.0.0.1:7890")
set(ENV{https_proxy} "http://127.0.0.1:7890")

set(OPENSSL_ROOT D:/work/lib/OpenSSL-Win64)

add_definitions(-DWIN32_LEAN_AND_MEAN)
add_definitions(-DNOMINMAX)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)