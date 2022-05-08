#include "apngimageplugin.h"
#include "apngimagehandler_p.h"
#include "apngreader_p.h"
#include "apngwriter_p.h"

ApngImagePlugin::ApngImagePlugin(QObject *parent) :
	QImageIOPlugin(parent)
{}

QImageIOPlugin::Capabilities ApngImagePlugin::capabilities(QIODevice *device, const QByteArray &format) const
{
    QImageIOPlugin::Capabilities caps = static_cast<Capability>(0);

    if (format == "apng" || format.isEmpty())
    {
        if (device->isWritable())
        {
            caps |= CanWrite;
        }
        if (device->isReadable() && device->bytesAvailable() >= 8 &&  ApngReader::checkPngSig(device))
        {
            caps |= CanRead;
        }
    }

    return caps;
}

QImageIOHandler *ApngImagePlugin::create(QIODevice *device, const QByteArray &format) const
{
    if(device && (capabilities(device, format).testFlag(CanRead) || capabilities(device, format).testFlag(CanWrite)))
    {
		auto handler = new ApngImageHandler();
		handler->setDevice(device);
		handler->setFormat(format);
		return handler;
    }
    return nullptr;
}
