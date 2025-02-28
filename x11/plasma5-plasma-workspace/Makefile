PORTNAME=	plasma-workspace
DISTVERSION=	${KDE_PLASMA_VERSION}.1
PORTREVISION=	3
CATEGORIES=	x11 kde kde-plasma

PATCH_SITES=	https://invent.kde.org/plasma/plasma-workspace/-/commit/
PATCHFILES=	e109503c8ee12866ecfbd3aaf89170a753f2ea84.patch:-p1 # Fix writing ICEAuthority file

MAINTAINER=	kde@FreeBSD.org
COMMENT=	Plasma5 Plasma workspace
WWW=		https://www.kde.org/plasma-desktop

BUILD_DEPENDS=	kio-extras>=23.08.5:devel/kio-extras \
		wayland-protocols>=1.31:graphics/wayland-protocols
LIB_DEPENDS=	libKUserFeedbackCore.so:deskutils/kuserfeedback \
		libcln.so:math/cln \
		libdbusmenu-qt5.so:devel/libdbusmenu-qt@qt5 \
		libfreetype.so:print/freetype2 \
		libfontconfig.so:x11-fonts/fontconfig \
		libicui18n.so:devel/icu \
		libqalculate.so:math/libqalculate \
		libwayland-client.so:graphics/wayland \
		libxkbcommon.so:x11/libxkbcommon \
		libxcb-image.so:x11/xcb-util-image \
		libxcb-util.so:x11/xcb-util
RUN_DEPENDS=	ck-launch-session:sysutils/consolekit2 \
		iceauth:x11/iceauth \
		kio-extras>=23.08.5:devel/kio-extras \
		xmessage:x11/xmessage \
		xrdb:x11/xrdb \
		xset:x11/xset \
		xsetroot:x11/xsetroot \
		${LOCALBASE}/libdata/pkgconfig/xkeyboard-config.pc:x11/xkeyboard-config \
		iso-codes>=0:misc/iso-codes \
		${LOCALBASE}/bin/genv:sysutils/coreutils \
		accountsservice>=0:sysutils/accountsservice \
		oxygen-icons>=0:x11-themes/oxygen-icons

USES=		cmake compiler:c++11-lib cpe desktop-file-utils gettext gl \
		kde:5 pkgconfig qt:5 tar:xz xorg
USE_GL=		gl
USE_KDE=	activities activities-stats activitymanagerd archive attica \
		auth baloo bookmarks codecs completion config configwidgets \
		coreaddons crash dbusaddons emoticons filemetadata globalaccel \
		guiaddons holidays i18n iconthemes idletime init itemmodels \
		itemviews jobwidgets js jsembed kcmutils kdeclarative kdesu \
		kio kquickcharts kscreenlocker kwin layer-shell-qt libkexiv2 \
		libkscreen libksysguard newstuff notifications notifyconfig \
		package parts people phonon plasma-framework plasma-integration \
		prison pty runner service solid sonnet syntaxhighlighting \
		texteditor textwidgets unitconversion wallet wayland \
		widgetsaddons windowsystem xmlgui xmlrpcclient \
		doctools:build ecm:build plasma-wayland-protocols:build \
		breeze-icons:run breeze:run kded:run kquickcharts:run \
		milou:run
USE_QT=		concurrent core dbus declarative graphicaleffects gui network \
		printsupport qdbus script sql svg wayland \
		widgets x11extras xml \
		buildtools:build qmake:build testlib:build \
		paths:run quickcontrols:run
USE_XORG=	ice sm x11 xau xcb xcomposite xcursor xext xfixes xft xi \
		xkbfile xrender xtst

CPE_VENDOR=	kde

CMAKE_OFF=	BUILD_TESTING

OPTIONS_DEFINE=		PIPEWIRE
OPTIONS_DEFAULT=	PIPEWIRE

PIPEWIRE_DESC=		Screen capture via PipeWire

PIPEWIRE_LIB_DEPENDS=	libpipewire-0.3.so:multimedia/pipewire
PIPEWIRE_USE=		KDE=kpipewire
PIPEWIRE_CMAKE_OFF=	-DDISABLE_PIPEWIRE:BOOL=ON

post-patch:
	@${REINPLACE_CMD} -e 's|%%LOCALBASE%%|${LOCALBASE}|g' \
		${PATCH_WRKSRC}/startkde/startplasma.cpp

post-stage:
	${INSTALL_SCRIPT} ${FILESDIR}/startplasma-wayland.sh ${STAGEDIR}/${LOCALBASE}/bin/

.include <bsd.port.mk>
