#pragma once

#include <QObject>
#include <QMainWindow>
#include <QOpenglWidget>
#include <QTimer>

class C_QWidgetRendererBase : public QOpenGLWidget
{
	Q_OBJECT
public:
	C_QWidgetRendererBase(QWidget * parent = 0);
	virtual ~C_QWidgetRendererBase() = default;

	// there is no reason for copy now
	// C_QWidgetRendererBase(const C_QWidgetRendererBase&) = delete;
	// C_QWidgetRendererBase & operator= (const C_QWidgetRendererBase&) = delete;


	int		GetFpsLimit() const { return m_fpsLimit; }
	void	SetFpsLimit(const int val) { m_fpsLimit = val; }

	void	RunUpdates(bool run = true);


protected:
	virtual void paintGL();
	virtual void resizeGL(int w, int h) = 0;
	virtual void initializeGL();
	virtual void beforeRendering() = 0;
	virtual void draw() = 0;

	bool	m_Init		: 1;
	bool	m_Updates	: 1;
	int		m_fpsLimit	= 30;
	QTimer	m_Timer;
};

