PORTNAME=	async-pool
PORTVERSION=	0.10.2
CATEGORIES=	www rubygems
MASTER_SITES=	RG

MAINTAINER=	sunpoet@FreeBSD.org
COMMENT=	Redis client library
WWW=		https://socketry.github.io/async-pool/ \
		https://github.com/socketry/async-pool

LICENSE=	MIT
LICENSE_FILE=	${WRKSRC}/license.md

RUN_DEPENDS=	rubygem-async>=1.25:devel/rubygem-async \
		rubygem-traces>=0:devel/rubygem-traces

USES=		gem

NO_ARCH=	yes

.include <bsd.port.mk>
