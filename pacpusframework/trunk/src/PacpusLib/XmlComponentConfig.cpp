// %pacpus:license{
// This file is part of the PACPUS framework distributed under the
// CECILL-C License, Version 1.0.
// %pacpus:license}
/// @author  Gerald Dherbomez <firstname.surname@utc.fr>
/// @version    $Id: XmlComponentConfig.cpp 118 2013-06-26 16:16:28Z morasjul $

#include <Pacpus/kernel/XmlComponentConfig.h>

#include <Pacpus/kernel/Log.h>
#include <Pacpus/kernel/XmlConfigFile.h>

using namespace pacpus;
using namespace std;

DECLARE_STATIC_LOGGER("pacpus.core.XmlComponentConfig");

static const string kPropertyComponentType = "type";

XmlComponentConfig::XmlComponentConfig(const QString& name)
{
    LOG_TRACE("XmlComponentConfig(QString)");

    // Get the pointer to the document and create the component.
    parentDocument_ = XmlConfigFile::create();
    if (name != QString::null)
      component_ = parentDocument_->createComponent(name);
}

/// Destructor.
XmlComponentConfig::~XmlComponentConfig()
{
    LOG_TRACE("~XmlComponentConfig()");
}

void XmlComponentConfig::addProperty(const QString& name)
{
    if (hasProperty(name))
    {
        LOG_ERROR("cannot add component property:"
                  << " component '" << component_.tagName() << "'"
                  << " already contains property '" << name << "'"
                  << " and its value is '" << component_.attribute(name) << "'"
                  );
        return;
    }

    // The property does not exist, it can be added.
    component_.setAttribute(name, 0);
    LOG_INFO("property '" << name << "'"
              << " was added to the component " << component_.tagName() << "'"
              << " and set to '" << component_.attribute(name) << "'"
              );
}

int XmlComponentConfig::delProperty(const QString& name)
{
    if (!hasProperty(name)) {
        LOG_WARN("cannot delete component property '" << name << "'"
                 << " of component '" << component_.tagName() << "'"
                 << ". Component does not contain this property."
                 );
        return false;
    }

    // The property exists, it can be removed.
    component_.removeAttribute(name);
    LOG_INFO("property '" << name << "' "
      << " of component '" << component_.tagName() << "'"
      << " was deleted"
      );

    return true;
}

QString XmlComponentConfig::getProperty(const QString& name, const QString& defaultValue) const
{
    if (!hasProperty(name))
    {
        LOG_WARN("cannot retrieve component property '" << name << "'"
                 << " of component '" << component_.tagName() << "'"
                 << ". Component does not contain this property."
                 );
        return defaultValue;
    }

    // The property exists, the value can be retrieved.
    return component_.attribute(name);
}

bool XmlComponentConfig::getBoolProperty(const QString& name, bool defaultValue) const
{
  return hasProperty(name) ? getProperty(name) == "true" : defaultValue;
}

int XmlComponentConfig::getIntProperty(const QString& name, int defaultValue) const
{
  return hasProperty(name) ? getProperty(name).toInt() : defaultValue;
}

double XmlComponentConfig::getDoubleProperty(const QString& name, double defaultValue) const
{
  return hasProperty(name) ? getProperty(name).toDouble() : defaultValue;
}

void XmlComponentConfig::setProperty(const QString& name, const QString& value)
{
    component_.setAttribute(name, value);
    LOG_INFO("property " << name
             << " of the component " << component_.tagName()
             << " was set to : " << value
             );
}

bool XmlComponentConfig::hasProperty(const QString& name) const
{
    return component_.hasAttribute(name);
}

QDomElement XmlComponentConfig::qDomElement() const
{
    return component_;
}

void XmlComponentConfig::localCopy(const QDomElement& elementToCopy)
{
    component_ = elementToCopy;
}

QString const XmlComponentConfig::getComponentName() const
{
    return component_.tagName();
}

QString const XmlComponentConfig::getComponentType() const
{
    return getProperty(kPropertyComponentType.c_str());
}