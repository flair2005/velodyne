// Autogenerated file by PacpusPlugin.cmake
// DO NOT EDIT!!! ALL CHANGES WOULD BE REMOVED BY THE NEXT CALL OF CMAKE

#ifndef __VELODYNEPCLVIEWERPLUGIN_H__
#define __VELODYNEPCLVIEWERPLUGIN_H__

#include <QObject>
#include <qplugin.h>

#include <Pacpus/kernel/PacpusPluginInterface.h>

/// Auto-generated plugin class
class VelodynePCLViewerPlugin
    : public QObject
    , public PacpusPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PacpusPluginInterface)

public:
    VelodynePCLViewerPlugin();
    ~VelodynePCLViewerPlugin();

protected:
    QString name();
};

#endif // __VELODYNEPCLVIEWERPLUGIN_H__
