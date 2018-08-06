#ifndef EDITOR_H
#define EDITOR_H

#include <QtWidgets/QMainWindow>
#include <QTextEdit>
#include <QCompleter>
#include <QObject>

#include <ui_editor.h>

class C_GLWidget;

class C_Editor : public QMainWindow
{
	Q_OBJECT

public:
	C_Editor(QWidget *parent = 0);
	~C_Editor() = default;

	// there is no reason for copy now
	C_Editor(const C_Editor &) = delete;
	C_Editor & operator= (const C_Editor&) = delete;

private:
	C_GLWidget*		m_glWidget;
	Ui::EditorClass ui;
};
#endif // EDITOR_H
