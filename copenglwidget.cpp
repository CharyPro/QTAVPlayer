#include "copenglwidget.h"

#include <QTimer>

extern "C" {
#include <libavutil/frame.h>
}
#define V_VER     3
#define T_VER     4
#define GET_STR(x)  #x
//#define M_DEBUG

///
/// 对于使用OpenGL来显示视频，原理是使用边界的四个顶点组成四边形作为显示的区域，将视频的每一帧图片当作材质设置在四边形上
///
COpenGLWidget::COpenGLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{

}

COpenGLWidget::~COpenGLWidget()
{

}

void COpenGLWidget::Play()
{
    playStatus = true;
}

void COpenGLWidget::Init(int width, int height)
{
    this->m_width = width;
    this->m_height = height;
    qDebug() << __FUNCTION__ << width << " " << height;

    delete datas[0];
    delete datas[1];
    delete datas[2];
    ///分配材质内存空间
    datas[0] = new char[m_width*m_height];		//Y
    datas[1] = new char[m_width*m_height / 4];	//U
    datas[2] = new char[m_width*m_height / 4];	//V

    InitTexture();
}

void COpenGLWidget::Repaint(AVFrame *frame)
{
    //容错，保证尺寸正确
    if (!datas[0] || m_width*m_height == 0 || frame->width != m_width || frame->height != m_height)
    {
        av_frame_free(&frame);
        return;
    }

    if (m_width == frame->linesize[0]) //无需对齐
    {
        memcpy(datas[0], frame->data[0], m_width*m_height);
        memcpy(datas[1], frame->data[1], m_width*m_height / 4);
        memcpy(datas[2], frame->data[2], m_width*m_height / 4);
    }
    else//行对齐问题
    {
        for (int i = 0; i < m_height; i++) //Y
            memcpy(datas[0] + m_width * i, frame->data[0] + frame->linesize[0] * i, m_width);
        for (int i = 0; i < m_height / 2; i++) //U
            memcpy(datas[1] + m_width / 2 * i, frame->data[1] + frame->linesize[1] * i, m_width);
        for (int i = 0; i < m_height / 2; i++) //V
            memcpy(datas[2] + m_width / 2 * i, frame->data[2] + frame->linesize[2] * i, m_width);
    }
    av_frame_free(&frame);

    qDebug() << __FUNCTION__ << m_width << " " << m_height;

    playStatus = true;

    //刷新显示
    update();
}

void COpenGLWidget::paintGL()
{
#ifdef M_DEBUG
    if(!playStatus) {
        QOpenGLWidget::paintGL();
        return;
    }
    if(m_fp.atEnd())
        return;//m_fp.seek(0);
    qDebug() << __FUNCTION__;

    m_fp.read(datas[0], m_width*m_height);
    m_fp.read(datas[1], m_width*m_height/4);
    m_fp.read(datas[2], m_width*m_height/4);

#else
    if(!playStatus && datas[0] == 0 ) {
        QOpenGLWidget::paintGL();
        return;
    }
#endif
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texs[0]); //0层绑定到Y材质
    //修改材质内容(复制内存内容)
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RED, GL_UNSIGNED_BYTE, datas[0]);
    //与shader uni遍历关联
    glUniform1i(tex_uniform[0], 0);


    glActiveTexture(GL_TEXTURE0+1);
    glBindTexture(GL_TEXTURE_2D, texs[1]); //1层绑定到U材质
                                           //修改材质内容(复制内存内容)
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width/2, m_height / 2, GL_RED, GL_UNSIGNED_BYTE, datas[1]);
    //与shader uni遍历关联
    glUniform1i(tex_uniform[1],1);


    glActiveTexture(GL_TEXTURE0+2);
    glBindTexture(GL_TEXTURE_2D, texs[2]); //2层绑定到V材质
                                           //修改材质内容(复制内存内容)
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width / 2, m_height / 2, GL_RED, GL_UNSIGNED_BYTE, datas[2]);
    //与shader uni遍历关联
    glUniform1i(tex_uniform[2], 2);

    glDrawArrays(GL_TRIANGLE_STRIP,0,4);
}

void COpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();//初始化opengl

    qDebug() << m_program.addShaderFromSourceFile(QGLShader::Vertex, ":/shader/shapes.vert");
    qDebug() << m_program.addShaderFromSourceFile(QGLShader::Fragment, ":/shader/shapes.frag");
//    qDebug() << m_program.addShaderFromSourceCode(QGLShader::Vertex, vstring);
//    qDebug() << m_program.addShaderFromSourceCode(QGLShader::Fragment, tstring);
    qDebug() << "program.link:" << m_program.link();

    //设置顶点坐标变量
    m_program.bindAttributeLocation("vertexIn", V_VER);
    //设置材质坐标
    m_program.bindAttributeLocation("textureIn", T_VER);

    //编译shader
    qDebug() << "program.bind:" << m_program.bind();

    //顶点  四边形  顶点坐标是在世界坐标系上
    static const GLfloat ver[] = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        -1.0f, 1.0f,
        1.0f, 1.0f
    };

    //材质  纹理的生成是由图片像素来生成的，而图像的存储是从左上角开始，对应于纹理的左下角处，即图像像素坐标于纹理坐标上下颠倒
    static const GLfloat tex[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f
    };

    glVertexAttribPointer(V_VER, 2, GL_FLOAT, false, 0, ver);
    glEnableVertexAttribArray(V_VER);
    glVertexAttribPointer(T_VER, 2, GL_FLOAT, false, 0, tex);
    glEnableVertexAttribArray(T_VER);

    tex_uniform[0] = m_program.uniformLocation("tex_y");
    tex_uniform[1] = m_program.uniformLocation("tex_u");
    tex_uniform[2] = m_program.uniformLocation("tex_v");

    //创建材质
    glGenTextures(3, texs);


#ifdef M_DEBUG
    InitTexture();

    ///分配材质内存空间
    datas[0] = new char[m_width*m_height];		//Y
    datas[1] = new char[m_width*m_height/4];	//U
    datas[2] = new char[m_width*m_height/4];	//V

    m_fp.setFileName("G:/out_resu.yuv");
    if(!m_fp.open(QFile::ReadOnly))
        qDebug() << "out240x128.yuv file open failed!";

    //启动定时器
    QTimer *ti = new QTimer(this);
    connect(ti, SIGNAL(timeout()), this, SLOT(update()));
    ti->start(40);
    playStatus = true;
#endif
}

void COpenGLWidget::InitTexture()
{
    // TODO： 此处需要考虑图片格式YUV420或者其它的，下面暂时当作YUV420处理
    //Y
    glBindTexture(GL_TEXTURE_2D, texs[0]);
    //放大过滤，线性插值   GL_NEAREST(效率高，但马赛克严重)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //创建材质显卡空间
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_width, m_height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

    //U
    glBindTexture(GL_TEXTURE_2D, texs[1]);
    //放大过滤，线性插值
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //创建材质显卡空间
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_width/2, m_height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

    //V
    glBindTexture(GL_TEXTURE_2D, texs[2]);
    //放大过滤，线性插值
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //创建材质显卡空间
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_width / 2, m_height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

}

void COpenGLWidget::resizeGL(int w, int h)
{

}


