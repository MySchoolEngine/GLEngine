#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>

#include "QWidgetRendererBase.h"

class C_GLWidget : public C_QWidgetRendererBase
{
	Q_OBJECT

public:
	C_GLWidget(QWidget *parent = 0);
	virtual ~C_GLWidget();

	// there is no reason for copy now
	C_GLWidget(const C_GLWidget &) = delete;
	C_GLWidget & operator= (const C_GLWidget&) = delete;

	virtual QSize sizeHint() const override;

protected:
	virtual void Cleanup();
	virtual void initializeGL() override;
	virtual void paintGL() override;
	virtual void resizeGL(int width, int height) override;
	virtual void mousePressEvent(QMouseEvent *event) override;
	virtual void mouseMoveEvent(QMouseEvent *event) override;


	virtual void beforeRendering() override;
	virtual void draw() override;

private:
	float m_red;
	QPoint m_lastPos;

	bool m_core;
};
