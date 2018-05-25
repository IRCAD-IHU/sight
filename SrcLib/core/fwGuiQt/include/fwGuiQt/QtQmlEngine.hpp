#pragma once

#include "fwGuiQt/config.hpp"

#include <fwServices/IQmlEngine.hpp>

#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QObject>
#include <QQuickWindow>

#include <memory>
#include <vector>

namespace fwGuiQt
{

	/**
	 *	@brief: The purpose is to load `scriptFile`.qml and display it
	 *
	 */
class FWGUIQT_CLASS_API QtQmlEngine : public ::fwServices::IQmlEngine, public QQmlApplicationEngine
{
private:
	static QtQmlEngine	*m_qtQmlEngine;

public:
	FWGUIQT_API QtQmlEngine();
    FWGUIQT_API ~QtQmlEngine();

    static FWGUIQT_API QtQmlEngine&	getEngine();
	
	/**
	 *	@brief: This function load file specified by scriptFile
	 *		This call can throw if the file contain a error or if it can't be found
	 */
    FWGUIQT_API void	loadFile(std::string const& scriptFile);
	/**
	 *	@brief: Run script file
	 */
    FWGUIQT_API void	launch();

	/**
	 *	@brief: return root window
	 */
    FWGUIQT_API QQuickWindow	*getWindow() const;

	/**
	 * @brief: This function will instanciate a class derived of QObject
	 * @uid: Name of the context (to be used in QML)
	 * @type: Class name
	 */
    virtual FWGUIQT_API void 	addCtx(std::string const& uid, std::string const& type);

    /**
     *  @brief: Call IQmlService::stop() method
     */
    FWGUIQT_API void    stopServices();

    FWGUIQT_API void    stop(int status) override;


private:

    /**
     * @brief: Run autoStarted service
     * Call IQmlService::configure() then IQmlService::start()
     */
    FWGUIQT_API void    runServices();

private:
	std::string	m_scriptFile;
	std::unique_ptr<QQmlComponent>	m_component;
	std::vector<std::unique_ptr<QObject> >	m_context;
    QQuickWindow	*m_rootWindow;
};

} // fwGuiQt