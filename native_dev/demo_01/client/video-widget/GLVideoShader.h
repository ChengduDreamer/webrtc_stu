#ifndef GL_VIDEO_WIDGET_SHADER_H
#define GL_VIDEO_WIDGET_SHADER_H

static const char* kVertexShader = R"(

    #version 330 core
    in vec3 aPos;
    in vec3 aColor;
    in vec2 aTex;

    out vec3 outColor;
    out vec2 outTex;

    void main()
    {   
        // Y 乘以 -1.0 作用是反转 Y坐标，默认情况OpenGL的坐标显示的是相反的
        gl_Position = vec4(aPos.x, aPos.y * -1.0, aPos.z, 1.0);
        outColor = aColor;
        outTex = aTex;
    }

)";


static const char* kNV12FragmentShader = R"(

    #version 330 core

    in vec3 outColor;
    in vec2 outTex;

    uniform sampler2D image1;
    uniform sampler2D image2;

    const vec3 delyuv = vec3(-16.0/255.0,-128.0/255.0,-128.0/255.0);
    const vec3 matYUVRGB1 = vec3(1.164, 0.0, 1.596);
    const vec3 matYUVRGB2 = vec3(1.164, -0.391, -0.813);
    const vec3 matYUVRGB3 = vec3(1.164, 2.018, 0.0);

    out vec4 FragColor;

    void main()
    {
        vec4 yColor = texture(image1, outTex);
        vec4 uvColor = texture(image2, outTex);

        highp vec3 yuv;
        vec3 CurResult;

        yuv.x = yColor.r;
        yuv.y = uvColor.r;
        yuv.z = uvColor.a;

        yuv += delyuv;

        CurResult.x = dot(yuv,matYUVRGB1);
        CurResult.y = dot(yuv,matYUVRGB2);
        CurResult.z = dot(yuv,matYUVRGB3);

        FragColor = vec4(CurResult.rgb, 1);
    }

)";

static const char* kRGBFragmentShader = R"(

    #version 330

    in vec3 outColor;
    in vec2 outTex;

    uniform sampler2D image1;
    out vec4 FragColor;

    void main()
    {   
        vec4 color = texture(image1, outTex);
        // 一般情况下不用额外处理，当红蓝反色时，注意RGB 与 BGR调换
        // FragColor = vec4(color.bgr, 1);
        FragColor = vec4(color.rgb, 1);
        //FragColor = vec4(1.0, 0.6, 0.3, 1);
    }

)";

static const char* kI420FragmentShader = R"(

    #version 330

    in vec3 outColor;
    in vec2 outTex;

    uniform sampler2D imageY;
    uniform sampler2D imageU;
    uniform sampler2D imageV;

	uniform int flipX;

    out vec4 FragColor;

    void main()
    {   
        float y, u, v, r, g, b;
		vec2 texCoord = outTex;
		
		if (flipX == 1) {
			vec2 flipXTex = vec2(outTex.x * -1.0, outTex.y);
			texCoord = flipXTex;
		}

        y = texture(imageY, texCoord).r;
        u = texture(imageU, texCoord).r;
        v = texture(imageV, texCoord).r;        
        
        y = 1.164 * (y - 16.0 / 255.0);
        u = u - 128.0 / 255.0;
        v = v - 128.0 / 255.0;

        r = y + 1.596 * v;
        g = y - 0.391 * u - 0.813 * v;
        b = y + 2.018 * u;

        FragColor = vec4(r, g, b, 1.0); 
    }
)";

#endif // TEX_SHADER_H
