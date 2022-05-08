#ifndef APNGIMAGEHANDLER_P_H
#define APNGIMAGEHANDLER_P_H

#include <QImageIOHandler>
#include <QVariant>
#include <QImage>
#include <QVector>
class ApngReader;
class ApngWriter;

class ApngImageHandler : public QImageIOHandler
{
	Q_DISABLE_COPY(ApngImageHandler)
public:
	ApngImageHandler();
	~ApngImageHandler() override;

	// QImageIOHandler interface
	QByteArray name() const override;
	bool canRead() const final;
	bool read(QImage *image) final;
    bool write(const QImage &image) final;
	QVariant option(ImageOption option) const final;
	bool supportsOption(ImageOption option) const final;
	bool jumpToNextImage() final;
	bool jumpToImage(int imageNumber) final;
	int loopCount() const final;
	int imageCount() const final;
	int nextImageDelay() const final;
	int currentImageNumber() const final;
private:
	quint32 _index = 0;
	QScopedPointer<ApngReader> _reader;
    QScopedPointer<ApngWriter> _writer;
};

#endif // APNGIMAGEHANDLER_P_H
