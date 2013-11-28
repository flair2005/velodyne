// %pacpus:license{
// This file is part of the PACPUS framework distributed under the
// CECILL-C License, Version 1.0.
// %pacpus:license}
/// @author  Gerald Dherbomez <firstname.surname@utc.fr>
/// @version    $Id: XmlConfigFile.cpp 91 2013-05-19 10:32:48Z gdherbom $

#include <Pacpus/kernel/XmlConfigFile.h>
#include <Pacpus/kernel/Log.h>

#include <QFile>
#include <QTextStream>

using namespace pacpus;
using namespace std;

DECLARE_STATIC_LOGGER("pacpus.core.XmlConfigFile");

XmlConfigFile * XmlConfigFile::_xmlConfigFile = NULL;

static const string kPropertyPluginList = "list";

static const string kXmlConfigFilename = "pacpus_config.xml";

#define componentSection "components"
#define parameterSection "parameters"
#define rootSection "pacpus"
#define pluginsNode "plugins"

XmlConfigFile::XmlConfigFile()
    : _file(NULL)
    , _numberOfComponents(0)
{
    LOG_TRACE("constructor");

    // create the root of the XML tree
    _document.appendChild(_document.createElement(rootSection));
    // create the sections
    _document.documentElement().appendChild(_document.createElement(parameterSection));
    _document.documentElement().appendChild(_document.createElement(componentSection));
    _file = new QFile(kXmlConfigFilename.c_str());
    if (NULL != _file) {
        LOG_INFO("XML document " << kXmlConfigFilename.c_str() << " was created");
    } else {
        LOG_WARN("cannot create XML document " << kXmlConfigFilename.c_str());
    }
}

XmlConfigFile::~XmlConfigFile()
{
    LOG_TRACE("destructor");
}

XmlConfigFile * XmlConfigFile::create()
{
    if (NULL ==_xmlConfigFile) {
        _xmlConfigFile = new XmlConfigFile();
    }
    return _xmlConfigFile;
}

void XmlConfigFile::destroy()
{
    delete _xmlConfigFile;
    _xmlConfigFile = NULL;
}

void XmlConfigFile::addComponent(QDomElement component)
{ 
    _mutex.lock();

    if (_document.documentElement().namedItem(componentSection).namedItem(component.tagName()).isNull()) {
        LOG_WARN("component " << component.tagName() << " exists already in the document");
    } else {
        QDomNode node = _document.documentElement().namedItem(componentSection).appendChild(component);
        ++_numberOfComponents;
        LOG_INFO("component " << node.nodeName() << " has been added to the section "
                 << _document.documentElement().namedItem(componentSection).nodeName());
    }
    _mutex.unlock();
}

void XmlConfigFile::delComponent(QDomElement component)
{
    _mutex.lock();

    QDomNode node = _document.documentElement().namedItem(componentSection).removeChild(component);
    if (node.isNull()) {
        LOG_WARN("component " << component.tagName() << " doesn't exist in the document.");
    } else {
        LOG_INFO("component " << node.nodeName() << " has been removed from the section "
                 << _document.documentElement().namedItem(componentSection).nodeName());
        --_numberOfComponents;
    }

    _mutex.unlock();
}

QDomElement XmlConfigFile::createComponent(QString name)
{
    LOG_DEBUG("creating component " << name);

    QMutexLocker mutexLocker(&_mutex);
    Q_UNUSED(mutexLocker);
    return _document.createElement(name);
}

void XmlConfigFile::saveFile(QString fileName)
{
    QMutexLocker mutexLocker(&_mutex);
    Q_UNUSED(mutexLocker);

    _file->setFileName(fileName);
    {
        _file->open(QIODevice::WriteOnly);
        {
            QTextStream ts(_file);
            ts << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n" + _document.toString();
        }
        _file->close();
    }
    LOG_DEBUG("file \"" << _file->fileName() << "\" has been saved");
}

int XmlConfigFile::loadFile(QString fileName)
{
    QMutexLocker mutexLocker(&_mutex);
    Q_UNUSED(mutexLocker);

    int line = 0, col = 0;
    QString errorMsg;

    if (_numberOfComponents != 0) {
        LOG_WARN("XML document contained " << _numberOfComponents << " components that will be lost!");
    }

    _file->setFileName(fileName);
    if (!_file->open( QIODevice::ReadOnly )) {
        LOG_ERROR("cannot open file " << fileName);
        return 0;
    }
    if (!_document.setContent( _file, true, &errorMsg, &line, &col )) {
        LOG_ERROR("cannot get data into the file " << fileName);
        LOG_ERROR(errorMsg << " at position " << line << ":" << col << " (line:col)");
        _file->close();
        return 0;
    }
    _file->close();

    // get the number of components in the loaded tree
    _numberOfComponents = _document.documentElement().namedItem(componentSection).childNodes().count();

    LOG_INFO("XML file \"" << fileName << "\" has been loaded. Number of components = " << _numberOfComponents);
    LOG_DEBUG("XML file content:\n"
              << "BEGIN============================================================================\n"
              << _document.toString()
              << "END==============================================================================\n"
              );

    return _numberOfComponents;
}

QDomElement XmlConfigFile::getComponent(QString componentName)
{
    LOG_DEBUG("getting component " << componentName);

    QDomNode node = _document.documentElement().namedItem(componentSection).namedItem(componentName);
    if (node.isNull()) {
        LOG_WARN("cannot get component " << componentName << ": document does not contain the component");
    }
    return node.toElement();
} 

QStringList XmlConfigFile::getAllComponents()
{
    QStringList componentNameList;
    QDomNode xmlNode;
    xmlNode = _document.documentElement().namedItem(componentSection).firstChild();

    for (int i = 0; i < _numberOfComponents; i++) {
        componentNameList.append(xmlNode.toElement().tagName());
        xmlNode = xmlNode.nextSibling();
    }
    return componentNameList;
}

QStringList XmlConfigFile::getAllPlugins()
{
    QDomElement node = _document.documentElement().namedItem(parameterSection).namedItem(pluginsNode).toElement();

    if (!node.hasAttribute(kPropertyPluginList.c_str())) {
        LOG_WARN("cannot retrieve plugins list, node 'plugins' may be misformed or attribute '" << kPropertyPluginList.c_str() << "' may be missing. "
                 << "Use the character pipe '|' to separate the different plugins ");
        return QStringList();
    } else {
        QString plugins = node.attribute(kPropertyPluginList.c_str());
        if (plugins.isEmpty()) {
            LOG_WARN("no plugins were specified");
            return QStringList();
        } else {
            return plugins.split("|", QString::SkipEmptyParts);
        }
    }
}