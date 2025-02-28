PORTNAME=	tellico
DISTVERSION=	4.0.1
PORTREVISION=	1
CATEGORIES=	misc kde
MASTER_SITES=	http://tellico-project.org/files/

MAINTAINER=	kde@FreeBSD.org
COMMENT=	Collection manager for KDE
WWW=		https://tellico-project.org

LICENSE=	GPLv2

LIB_DEPENDS=	libbtparse.so:textproc/p5-Text-BibTeX \
		libcdio.so:sysutils/libcdio \
	 	libcsv.so:textproc/libcsv \
		libexempi.so:textproc/exempi \
		libiso9660.so:sysutils/libcdio \
		libpoppler-qt5.so:graphics/poppler-qt5 \
		libpoppler.so:graphics/poppler \
		libtag.so:audio/taglib \
		libyaz.so:net/yaz

USES=		cmake compiler:c++11-lang desktop-file-utils gettext gnome \
		kde:5 pkgconfig python:run qt:5 shared-mime-info shebangfix \
		tar:xz xorg
USE_GNOME=	libxml2 libxslt
USE_KDE=	archive attica auth bookmarks codecs completion config \
		configwidgets coreaddons crash filemetadata guiaddons \
		i18n iconthemes itemmodels itemviews jobwidgets kio \
		libkcddb libksane newstuff service solid sonnet \
		textwidgets wallet widgetsaddons windowsystem xmlgui \
		doctools:build ecm:build
USE_QT=		charts concurrent core dbus declarative gui location network \
		printsupport webchannel webengine widgets xml \
		buildtools:build qmake:build testlib:build
USE_XORG=	x11

SHEBANG_FILES=	src/fetch/scripts/dark_horse_comics.py
CMAKE_OFF=	BUILD_WITH_QT6

.include <bsd.port.mk>
