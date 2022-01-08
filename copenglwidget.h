#ifndef COPENGLWIDGET_H
#define COPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QGLShaderProgram>
#include <fstream>
#include <QFile>

class COpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT

public:
    COpenGLWidget(QWidget *parent);
    ~COpenGLWidget();

    void Play();

protected:
    virtual void paintGL() Q_DECL_OVERRIDE;
    virtual void initializeGL() Q_DECL_OVERRIDE;
    virtual void resizeGL(int w, int h) Q_DECL_OVERRIDE;

private:
    bool playStatus = false;
    FILE* fp;
    QString m_videofile;
    QFile m_fp;

    QGLShaderProgram m_program;

    GLuint tex_uniform[3] = {0};
    GLuint texs[3];

//    int m_width, m_height;
    int m_width = 240;
    int m_height = 128;
    //材质内存空间
    char *datas[3] = { 0 };
};

#endif // COPENGLWIDGET_H
