# Copyright 1999-2008 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2

inherit eutils

SLOT="0"
LICENSE="GPL-3"
KEYWORDS="~alpha ~amd64 ~arm ~hppa ~ia64 ~m68k ~mips ~ppc ~ppc64 ~ppc-macos ~s390 ~sh ~sparc x86 ~x86-fbsd"
DESCRIPTION="Mount the Protein Data Bank as a filesystem with FUSE"
SRC_URI="http://pdbfs.googlecode.com/files/pdbfs-0.0.3.tar.gz"
HOMEPAGE="http://code.google.com/p/pdbfs/"
IUSE=""

DEPEND="sys-libs/glibc sys-fs/fuse sys-libs/zlib"
RDEPEND="${DEPEND} net-misc/rsync"

src_compile() {
	cd pdbfs-0.0.3
	econf || die "econf failed"
	emake || die "emake failed"
}

src_install() {
	cd pdbfs-0.0.3
	emake DESTDIR="${D}" install || die "emake install failed"
	dodoc README NEWS ChangeLog AUTHORS
}
