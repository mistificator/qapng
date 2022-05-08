#ifndef APNGREADER_P_H
#define APNGREADER_P_H

#include <QObject>
#include <QIODevice>
#include <png.h>
#include <QImage>

#include "apngtypes_p.h"

#ifndef PNG_APNG_SUPPORTED
#error libpng with APNG patch is required
#endif
#ifndef PNG_READ_APNG_SUPPORTED
#error libpng with APNG patch and APNG read support is required
#endif

class ApngReader : public QObject
{
	Q_OBJECT

public:
	explicit ApngReader(QObject *parent = nullptr);
	~ApngReader() override;

	static bool checkPngSig(QIODevice *device);

	bool init(QIODevice *device);
	ApngFrame readFrame(quint32 index);
	ApngFrame readFrame(int index);

	bool isAnimated() const;
	QSize size() const;
	quint32 frames() const;
	quint32 plays() const;

private:
	QIODevice *_device = nullptr;

	png_structp _png = nullptr;
	png_infop _info = nullptr;

	//image info
	qint64 _infoOffset = 0;
	bool _animated = false;
	bool _skipFirst = false;
	QSize _imageSize;
	quint32 _frameCount = 1;
	quint32 _plays = 0;

    Frame _frame;
	QImage _lastImg;

	QList<ApngFrame> _allFrames;
    QVector<QRgb> color_table;

	static void info_fn(png_structp png_ptr, png_infop info_ptr);
	static void row_fn(png_structp png_ptr, png_bytep new_row, png_uint_32 row_num, int pass);
	static void end_fn(png_structp png_ptr, png_infop info_ptr);

	static void frame_info_fn(png_structp png_ptr, png_uint_32 frame_num);
	static void frame_end_fn(png_structp png_ptr, png_uint_32 frame_num);

	bool readChunk(quint32 len = 0);
	void copyOver();
	void blendOver();
};

#endif // APNGREADER_P_H
