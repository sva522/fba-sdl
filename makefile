# Makefile for FBA, for use with GNU make
#
# The first pass makes sure all intermediary targets are present. The second pass updates
# any targets, if necessary. (Intermediary) targets which have their own unique rules
# are generated as required.

#
#	Flags. Uncomment any of these declarations to enable their function.
#

# Specify the name of the executable file, without ".exe"
NAME = fbasdl

# Check for changes in header files
DEPEND = 1

# Include features for debugging drivers
DEBUG = 1

# Perl is available
PERL = 1

# Choose emulation cores
BUILD_A68K = 1
BUILD_C68K = 1
BUILD_M68K = 1
USE_LIBAO = 1

#
#	Specify paths/files
#

objdir	= obj/linux/
srcdir	= src/

alldir	= burn \
		burn/devices \
		burn/drv \
		burn/drv/capcom \
		burn/drv/cave \
		burn/drv/coleco \
		burn/drv/cps3 \
		burn/drv/dataeast \
		burn/drv/galaxian \
		burn/drv/irem \
		burn/drv/konami \
		burn/drv/megadrive \
		burn/drv/midway \
		burn/drv/pce \
		burn/drv/neogeo \
		burn/drv/pgm \
		burn/drv/pre90s \
		burn/drv/psikyo \
		burn/drv/pst90s \
		burn/drv/sega \
		burn/drv/sg1000 \
		burn/drv/sms \
		burn/drv/snes \
		burn/drv/taito \
		burn/drv/toaplan \
		burn/snd \
		intf \
		intf/audio \
		intf/audio/sdl \
		intf/cd \
		intf/cd/sdl \
		intf/input \
		intf/input/sdl \
		intf/video \
		intf/video/sdl \
		intf/video/scalers \
		cpu \
		cpu/a68k \
		cpu/arm \
		cpu/arm7 \
		cpu/c68k \
		cpu/cz80 \
		cpu/h6280 \
		cpu/hd6309 \
		cpu/i8039 \
		cpu/konami \
		cpu/m68k \
		cpu/m6502 \
		cpu/m6800 \
		cpu/m6805 \
		cpu/m6809 \
		cpu/nec \
		cpu/pic16c5x \
		cpu/s2650 \
		cpu/sh2 \
		cpu/tlcs90 \
		cpu/z80 \
		dep/scripts \
		dep/generated \
		sdl-dingux

incdir	= $(foreach dir,$(alldir),-I$(srcdir)$(dir)) -I$(objdir)dep/generated -I/local/include -I/include/SDL

lib = -lstdc++ -lSDL -lSDL_image -lz

ifdef USE_LIBAO
	lib += -lao
endif

drvobj	+= \
		d_dodonpachi.o d_donpachi.o d_esprade.o d_feversos.o d_gaia.o d_guwange.o d_hotdogst.o d_korokoro.o d_mazinger.o \
		d_metmqstr.o d_pwrinst2.o d_sailormn.o d_tjumpman.o d_uopoko.o \
		\
		d_cps1.o \
		\
		d_cps2.o \
		\
		d_cps3.o \
		\
		d_actfancr.o d_backfire.o d_boogwing.o d_cbuster.o d_cninja.o d_darkseal.o d_dassault.o d_dec0.o d_dec8.o \
		d_dietgogo.o d_funkyjet.o d_karnov.o d_lemmings.o d_pktgaldx.o d_rohga.o d_sidepckt.o d_simpl156.o d_supbtime.o \
		d_tumblep.o d_vaportra.o \
		\
		d_galaxian.o \
		\
		d_m62.o d_m63.o d_m72.o d_m90.o d_m92.o d_m107.o d_vigilant.o \
		\
		d_88games.o d_ajax.o d_aliens.o d_asterix.o d_battlnts.o d_bishi.o d_bladestl.o d_blockhl.o d_bottom9.o d_circusc.o d_contra.o \
		d_crimfght.o d_dbz.o d_finalzr.o d_gberet.o d_gijoe.o d_gbusters.o d_gradius3.o d_gyruss.o d_hcastle.o d_hexion.o d_ironhors.o \
		d_jailbrek.o d_kontest.o d_labyrunr.o d_lethal.o d_mainevt.o d_megazone.o d_mikie.o d_mogura.o d_moo.o d_mystwarr.o d_nemesis.o \
		d_pandoras.o d_parodius.o d_pooyan.o d_rocnrope.o d_rockrage.o d_rollerg.o d_scotrsht.o d_shaolins.o d_simpsons.o d_spy.o \
		d_surpratk.o d_thunderx.o d_timeplt.o d_tmnt.o d_tp84.o d_tutankhm.o d_twin16.o d_ultraman.o d_vendetta.o d_xexex.o d_xmen.o \
		d_yiear.o \
		\
		d_neogeo.o \
		\
		d_pgm.o \
		\
		d_psikyo.o d_psikyo4.o d_psikyosh.o \
		\
		d_angelkds.o d_bankp.o d_dotrikun.o d_hangon.o d_outrun.o d_suprloco.o d_suprnova.o d_sys1.o d_sys16a.o d_sys16b.o d_sys18.o \
		d_xbrd.o d_ybrd.o d_zaxxon.o \
		\
		d_arkanoid.o d_ashnojoe.o d_asuka.o d_bublbobl.o d_chaknpop.o d_darius2.o d_darkmist.o d_exzisus.o d_flstory.o d_lkage.o \
		d_minivdr.o d_othunder.o d_retofinv.o d_slapshot.o d_superchs.o d_taitob.o d_taitof2.o d_taitomisc.o d_taitox.o d_taitoz.o \
		d_tnzs.o d_wyvernf0.o \
		\
		d_batrider.o d_batsugun.o d_battleg.o d_bbakraid.o d_demonwld.o d_dogyuun.o d_fixeight.o d_ghox.o d_hellfire.o \
		d_kbash.o d_kbash2.o d_mahoudai.o d_outzone.o d_pipibibs.o d_rallybik.o d_samesame.o d_shippumd.o d_slapfght.o \
		d_snowbro2.o d_tekipaki.o d_truxton.o d_truxton2.o d_vfive.o d_vimana.o d_zerowing.o \
		\
		d_4enraya.o d_1942.o d_1943.o d_alinvade.o d_alpha68k.o d_ambush.o d_arabian.o d_armedf.o d_atetris.o d_aztarac.o d_baraduke.o \
		d_bionicc.o d_blktiger.o d_blockout.o d_blueprnt.o d_bombjack.o d_capbowl.o d_commando.o d_cybertnk.o d_ddragon.o d_djboy.o d_dkong.o \
		d_dynduke.o d_epos.o d_exedexes.o d_funkybee.o d_galaga.o d_gauntlet.o d_ginganin.o d_gng.o d_gunsmoke.o d_higemaru.o \
		d_ikki.o d_invaders.o d_jack.o d_kangaroo.o d_kncljoe.o d_kyugo.o d_ladybug.o d_lwings.o d_lastduel.o d_mario.o d_markham.o \
		d_marineb.o d_megasys1.o d_meijinsn.o d_mitchell.o d_mole.o d_momoko.o d_mrdo.o d_mrflea.o d_mustache.o d_mystston.o d_ninjakd2.o \
		d_pacland.o d_pacman.o d_pac2650.o d_pkunwar.o d_prehisle.o d_punchout.o d_psychic5.o d_quizo.o d_rallyx.o d_renegade.o d_rpunch.o \
		d_route16.o d_scregg.o d_sf.o d_sidearms.o d_skyarmy.o d_skyfox.o d_skykid.o d_snk68.o d_solomon.o d_sonson.o d_srumbler.o d_suna8.o d_tail2nose.o \
		d_tbowl.o d_tecmo.o d_terracre.o d_tigeroad.o d_timelimt.o d_toki.o d_tsamurai.o d_vulgus.o d_wallc.o d_wc90.o d_wc90b.o d_wwfsstar.o \
		d_xain.o \
		\
		d_1945kiii.o d_aerofgt.o d_airbustr.o d_aquarium.o d_blmbycar.o d_bloodbro.o d_crospang.o d_crshrace.o d_dcon.o d_deniam.o \
		d_ddragon3.o d_diverboy.o d_dooyong.o d_drgnmst.o d_drtomy.o d_egghunt.o d_esd16.o d_f1gp.o d_funybubl.o \
		d_fuukifg3.o d_gaelco.o d_gaiden.o d_galpanic.o d_galspnbl.o d_gotcha.o d_gumbo.o d_hyperpac.o d_jchan.o d_kaneko16.o \
		d_lordgun.o d_mcatadv.o d_midas.o d_mugsmash.o d_mwarr.o d_news.o d_nmg5.o d_nmk16.o d_ohmygod.o d_pass.o d_pirates.o \
		d_playmark.o d_powerins.o d_pushman.o d_raiden.o d_raiden2.o d_sandscrp.o d_seta.o d_seta2.o d_shadfrce.o d_silkroad.o \
		d_silvmil.o d_speedspn.o d_suna16.o d_taotaido.o d_tecmo16.o d_tecmosys.o d_tetrisp2.o d_tumbleb.o d_unico.o d_vmetal.o \
		d_welltris.o d_wwfwfest.o d_xorworld.o d_xxmissio.o d_yunsun16.o d_zerozone.o \
		\
		d_parent.o \
		\
		#d_megadrive.o \
		#\
		#d_pce.o \
		#\
		#d_sms.o \
		#\
		#d_snes.o \
		#\
		#d_coleco.o \
		#\
		#d_sg1000.o

ifdef USE_LIBAO
	depobj = ao_audio.o 
endif

depobj += \
		bzip.o config.o drv.o font.o stringset.o stubs.o \
		input.o main.o xs.o lz4_all.o neocdlist.o paths.o run.o tchar.o \
		\
		sdl_audio.o sdl_input.o sdl_menu.o sdl_progress.o \
		sdl_run.o sdl_video.o snd.o \
		\
		ioapi.o unzip.o dat.o state.o zipfn.o \
		\
		gui_config.o gui_gfx.o gui_main.o gui_romlist.o gui_setpath.o \
		\
		interface.o \
		\
		cd_interface.o

depobj	+= \
		$(drvobj) \
		\
		burn.o burn_gun.o burn_led.o burn_memory.o burn_sound.o burn_sound_c.o cheat.o debug_track.o hiscore.o load.o \
		tiles_generic.o timer.o vector.o \
		\
		8255ppi.o 8257dma.o eeprom.o nmk004.o kaneko_tmap.o pandora.o seibusnd.o sknsspr.o slapstic.o t5182.o timekpr.o tms34061.o \
		v3021.o vdc.o tms9928a.o \
		\
		ay8910.o burn_y8950.o burn_ym2151.o burn_ym2203.o burn_ym2413.o burn_ym2608.o burn_ym2610.o burn_ym2612.o \
		burn_ym3526.o burn_ym3812.o burn_ymf278b.o c6280.o dac.o es5506.o es8712.o flt_rc.o fm.o fmopl.o ics2115.o iremga20.o \
		k005289.o k007232.o k051649.o k053260.o k054539.o msm5205.o msm5232.o msm6295.o namco_snd.o nes_apu.o rf5c68.o saa1099.o \
		samples.o segapcm.o sn76496.o upd7759.o vlm5030.o x1010.o ym2151.o ym2413.o ymdeltat.o ymf278b.o ymz280b.o \
		\
		arm7_intf.o arm_intf.o h6280_intf.o hd6309_intf.o konami_intf.o m6502_intf.o m6800_intf.o m6805_intf.o m6809_intf.o \
		m68000_intf.o nec_intf.o pic16c5x_intf.o s2650_intf.o tlcs90_intf.o z80_intf.o \
		\
		arm.o arm7.o h6280.o hd6309.o i8039.o konami.o m6502.o m6800.o m6805.o m6809.o nec.o pic16c5x.o s2650.o sh2.o tlcs90.o \
		v25.o z80.o z80daisy.o \
		\
		cave.o cave_palette.o cave_sprite.o cave_tile.o \
		\
		cps2_crpt.o cps.o cps_config.o cps_draw.o cps_mem.o cps_obj.o cps_pal.o cps_run.o cps_rw.o cps_scr.o cpsr.o cpsrd.o \
		cpst.o ctv.o fcrash_snd.o kabuki.o ps.o ps_m.o ps_z.o qs.o qs_c.o qs_z.o sf2mdt_snd.o \
		\
		cps3run.o cps3snd.o \
		\
		deco16ic.o \
		\
		gal_gfx.o gal_run.o gal_sound.o gal_stars.o \
		\
		irem_cpu.o \
		\
		k007342_k007420.o k051316.o k051733.o k051960.o k052109.o k053245.o k053247.o k053250.o k053251.o k053936.o k054000.o \
		k054338.o k055555.o k056832.o konamigx.o konamiic.o timeplt_snd.o \
		\
		neo_decrypt.o neo_palette.o neo_run.o neo_sprite.o neo_text.o neo_upd4990a.o neogeo.o \
		\
		pgm_crypt.o pgm_draw.o pgm_run.o pgm_asic3.o pgm_asic27a_type1.o pgm_asic27a_type2.o pgm_asic27a_type3.o pgm_asic25.o \
		\
		psikyo_palette.o psikyo_sprite.o psikyo_tile.o psikyosh_render.o \
		\
		fd1089.o fd1094.o genesis_vid.o mc8123.o sys16_fd1094.o sys16_gfx.o sys16_run.o \
		\
		cchip.o pc080sn.o pc090oj.o taito.o taito_ic.o taito_m68705.o tc0100scn.o tc0110pcr.o tc0140syt.o tc0150rod.o \
		tc0180vcu.o tc0220ioc.o tc0280grd.o tc0360pri.o tc0480scp.o tc0510nio.o tc0640fio.o tnzs_prot.o \
		\
		toa_bcu2.o toa_extratext.o toa_gp9001.o toa_palette.o toaplan1.o toaplan.o \
		\
		#megadrive.o \
		#\
		#pce.o \
		#\
		#sms.o smspio.o smssystem.o smsvdp.o smsfmintf.o smsrender.o smssound.o smstms.o \
		#\
		#snes_65816.o snes_io.o snes_main.o snes_ppu.o snes_spc700.o

ifdef	BUILD_M68K
ifdef	DEBUG
depobj += m68kdasm.o
endif
endif

autobj += $(depobj)

ifdef	BUILD_A68K
a68k.o	= $(objdir)cpu/a68k/a68k.o
endif

ifdef	BUILD_C68K
autobj += c68k.o
endif

autdep	= $(autobj:.o=.d)

driverlist.h = $(srcdir)dep/generated/driverlist.h
ctv.h	= $(srcdir)dep/generated/ctv.h
toa_gp9001_func.h = $(srcdir)dep/generated/toa_gp9001_func.h
neo_sprite_func.h = $(srcdir)dep/generated/neo_sprite_func.h
cave_tile_func.h = $(srcdir)dep/generated/cave_tile_func.h
cave_sprite_func.h = $(srcdir)dep/generated/cave_sprite_func.h
psikyo_tile_func.h = $(srcdir)dep/generated/psikyo_tile_func.h
pgm_sprite.h = $(srcdir)dep/generated/pgm_sprite.h
build_details.h = $(srcdir)dep/generated/build_details.h

ifdef BUILD_M68K
allobj = $(objdir)cpu/m68k/m68kcpu.o $(objdir)cpu/m68k/m68kops.o
endif

allobj += \
	  $(foreach file,$(autobj:.o=.c), \
		$(foreach dir,$(alldir),$(subst $(srcdir),$(objdir), \
		$(firstword $(subst .c,.o,$(wildcard $(srcdir)$(dir)/$(file))))))) \
	  $(foreach file,$(autobj:.o=.cpp), \
		$(foreach dir,$(alldir),$(subst $(srcdir),$(objdir), \
		$(firstword $(subst .cpp,.o,$(wildcard $(srcdir)$(dir)/$(file))))))) \
	  $(foreach file,$(autobj:.o=.asm), \
		$(foreach dir,$(alldir),$(subst $(srcdir),$(objdir), \
		$(firstword $(subst .asm,.o,$(wildcard $(srcdir)$(dir)/$(file)))))))

ifdef BUILD_A68K
allobj += $(a68k.o)
endif

alldep	= $(foreach file,$(autobj:.o=.c), \
		$(foreach dir,$(alldir),$(subst $(srcdir),$(objdir), \
		$(firstword $(subst .c,.d,$(wildcard $(srcdir)$(dir)/$(file))))))) \
	  $(foreach file,$(autobj:.o=.cpp), \
		$(foreach dir,$(alldir),$(subst $(srcdir),$(objdir), \
		$(firstword $(subst .cpp,.d,$(wildcard $(srcdir)$(dir)/$(file)))))))

#
#
#	Specify compiler/linker/assembler
#
#

HOSTCC = gcc
HOSTCXX = g++
CC	= gcc
CXX	= g++
LD	= $(CXX)
AS	= nasm

HOSTCFLAGS = $(incdir)
CFLAGS   = -O0 -fomit-frame-pointer -Wno-write-strings \
		-DLSB_FIRST
CXXFLAGS = -O0 -fomit-frame-pointer -Wno-write-strings \
		-DLSB_FIRST -fpermissive

CFLAGS += -U__cdecl -U__fastcall -D__cdecl="" -D__fastcall=""
CXXFLAGS += -U__cdecl -U__fastcall -D__cdecl="" -D__fastcall=""

ifdef USE_LIBAO
	CFLAGS += -DUSE_LIBAO
	CXXFLAGS += -DUSE_LIBAO
endif

DEF = -DUSE_SPEEDHACKS -DBUILD_SDL -DLINUX_PC

ifdef	DEBUG
	DEF += -DFBA_DEBUG -g
	HOSTCFLAGS += -DFBA_DEBUG -g
endif

ifdef BUILD_A68K
	DEF += -DBUILD_A68K
endif

ifdef BUILD_C68K
	DEF += -DBUILD_C68K
endif

ifdef BUILD_M68K
	DEF += -DBUILD_M68K
endif

DEF += -DFILENAME=$(NAME)

CFLAGS += $(DEF) $(incdir)
CXXFLAGS += $(DEF) $(incdir)

ifndef DEBUG
LDFLAGS += -s
endif

ASFLAGS = -O0 -f elf64

#
#
#	Specify paths
#
#

vpath %.asm	$(foreach dir,$(alldir),$(srcdir)$(dir)/ )
vpath %.cpp	$(foreach dir,$(alldir),$(srcdir)$(dir)/ )
vpath %.c	$(foreach dir,$(alldir),$(srcdir)$(dir)/ )
vpath %.h	$(foreach dir,$(alldir),$(srcdir)$(dir)/ )

vpath %.o 	$(foreach dir,$(alldir),$(objdir)$(dir)/ )
vpath %.d 	$(foreach dir,$(alldir),$(objdir)$(dir)/ )

#
#
#	Rules
#
#

.PHONY:	all init cleandep clean

ifeq ($(MAKELEVEL),0)
ifdef DEPEND

all:	init $(autdep) $(autobj)
	@$(MAKE) #-f Makefile.dingux-gcc
else

all:	init $(autobj)
	@$(MAKE) #-f Makefile.dingux-gcc
endif
else

all:	$(NAME)

endif

#
#
#	Rule for linking the executable
#
#

ifeq ($(MAKELEVEL),1)

$(NAME):	$(allobj)
	@echo
	@echo Linking executable $(NAME)...
	@mkdir -p bin
	@$(LD) $(CFLAGS) $(LDFLAGS) -o bin/$@ $^ $(lib)
	@mkdir -p bin/skin
	@cp src/sdl-dingux/skin/*.png bin/skin/
endif

#
#	Generate the gamelist
#

burn.o burn.d:	driverlist.h

$(driverlist.h): $(drvobj) $(srcdir)dep/scripts/gamelist.pl
ifdef	PERL
	@perl $(srcdir)dep/scripts/gamelist.pl -o $@ -l gamelist.txt \
		$(filter %.cpp,$(foreach file,$(drvobj:.o=.cpp),$(foreach dir,$(alldir), \
		$(firstword $(wildcard $(srcdir)$(dir)/$(file))))))
else
ifeq ($(MAKELEVEL),1)
	@echo
	@echo Warning: Perl is not available on this system.
	@echo $@ cannot be updated or created!
	@echo
endif
endif

#
#	Compile 68000 cores
#

# A68K

ifdef	BUILD_A68K
$(a68k.o):	fba_make68k.c
	@echo Compiling A68K MC68000 core...
	@$(HOSTCC) $(HOSTCFLAGS) $(LDFLAGS) -DWIN32 -Wno-unused -Wno-conversion -Wno-missing-prototypes \
		-s $< -o $(objdir)/dep/generated/fba_make68k
	@$(objdir)dep/generated/fba_make68k $(@:.o=.asm) \
		$(@D)/a68k_tab.asm 00 $(ppro)
	@echo Assembling A68K MC68000 core...
	@$(AS) $(ASFLAGS) $(@:.o=.asm) -o $@
endif

# Musashi

ifdef BUILD_M68K
$(objdir)cpu/m68k/m68kcpu.o: $(srcdir)cpu/m68k/m68kcpu.c $(objdir)dep/generated/m68kops.h $(srcdir)cpu/m68k/m68k.h $(srcdir)cpu/m68k/m68kconf.h
	@echo Compiling Musashi MC680x0 core \(m68kcpu.c\)...
	@$(CC) $(CFLAGS) -c $(srcdir)cpu/m68k/m68kcpu.c -o $(objdir)cpu/m68k/m68kcpu.o

$(objdir)cpu/m68k/m68kops.o: $(objdir)cpu/m68k/m68kmake $(objdir)dep/generated/m68kops.h $(objdir)dep/generated/m68kops.c $(srcdir)cpu/m68k/m68k.h $(srcdir)cpu/m68k/m68kconf.h
	@echo Compiling Musashi MC680x0 core \(m68kops.c\)...
	@$(CC) $(CFLAGS) -c $(objdir)dep/generated/m68kops.c -o $(objdir)cpu/m68k/m68kops.o

$(objdir)dep/generated/m68kops.h: $(objdir)cpu/m68k/m68kmake $(srcdir)cpu/m68k/m68k_in.c
	$(objdir)/cpu/m68k/m68kmake $(objdir)dep/generated/ $(srcdir)cpu/m68k/m68k_in.c

$(objdir)cpu/m68k/m68kmake: $(srcdir)cpu/m68k/m68kmake.c
	@echo Compiling Musashi MC680x0 core \(m68kmake.c\)...
	@$(HOSTCC) $(HOSTCFLAGS) $(srcdir)cpu/m68k/m68kmake.c -o $(objdir)cpu/m68k/m68kmake
endif

#
#	Extra rules for generated header file cvt.h, needed by ctv.cpp
#

ctv.d ctv.o:	$(ctv.h)

$(ctv.h):	ctv_make.cpp
	@echo Generating $(srcdir)dep/generated/$(@F)...
	@$(HOSTCXX) $(LDFLAGS) $< -o $(objdir)dep/generated/ctv_make
	@$(objdir)dep/generated/ctv_make >$@

#
#	Extra rules for generated header file toa_gp9001_func.h, needed by toa_gp9001.cpp
#

toa_bcu2.d toa_bcu2.o toa_gp9001.d toa_gp9001.o: $(toa_gp9001_func.h)

$(toa_gp9001_func.h):	$(srcdir)dep/scripts/toa_gp9001_func.pl
	@$(srcdir)dep/scripts/toa_gp9001_func.pl -o $(toa_gp9001_func.h)

#
#	Extra rules for generated header file neo_sprite_func.h, needed by neo_sprite.cpp
#

neo_sprite.d neo_sprite.o: $(neo_sprite_func.h)

$(neo_sprite_func.h):	$(srcdir)dep/scripts/neo_sprite_func.pl
	@$(srcdir)dep/scripts/neo_sprite_func.pl -o $(neo_sprite_func.h)

#
#	Extra rules for generated header file cave_tile_func.h, needed by cave_tile.cpp
#

cave_tile.d cave_tile.o: $(cave_tile_func.h)

$(cave_tile_func.h):	$(srcdir)dep/scripts/cave_tile_func.pl
	@perl $(srcdir)dep/scripts/cave_tile_func.pl -o $(cave_tile_func.h)

#
#	Extra rules for generated header file cave_sprite_func.h, needed by cave_sprite.cpp
#

cave_sprite.d cave_sprite.o: $(cave_sprite_func.h)

$(cave_sprite_func.h):	$(srcdir)dep/scripts/cave_sprite_func.pl
	@perl $(srcdir)dep/scripts/cave_sprite_func.pl -o $(cave_sprite_func.h)

#
#	Extra rules for generated header file psikyo_tile_func.h / psikyo_sprite_func.h, needed by psikyo_tile.cpp / psikyo_sprite.cpp
#

psikyo_tile.d psikyo_tile.o psikyosprite.d psikyo_sprite.o: $(psikyo_tile_func.h)

$(psikyo_tile_func.h):	$(srcdir)dep/scripts/psikyo_tile_func.pl
	@perl $(srcdir)dep/scripts/psikyo_tile_func.pl -o $(psikyo_tile_func.h)

#
#	Extra rules for generated header file pgm_sprite.h, needed by pgm_draw.cpp
#

pgm_draw.d pgm_draw.o:	$(pgm_sprite.h)

$(pgm_sprite.h):	pgm_sprite_create.cpp
	@echo Generating $(srcdir)dep/generated/$(@F)...
	@$(HOSTCXX) $(LDFLAGS) $< -o $(objdir)dep/generated/pgm_sprite_create
	@$(objdir)dep/generated/pgm_sprite_create >$@

ifeq ($(MAKELEVEL),1)
ifdef DEPEND

include	$(alldep)

endif
endif

#
#	Generic rules for C/C++ files
#

ifeq ($(MAKELEVEL),0)

%.o:	%.cpp
	@echo Compiling $<...
	@$(CXX) $(CXXFLAGS) -c $< -o $(subst $(srcdir),$(objdir),$(<D))/$(@F)

%.o:	%.c
	@echo Compiling $<...
	@$(CC) $(CFLAGS) -Wno-unused -Wno-conversion -Wno-missing-prototypes -c $< -o $(subst $(srcdir),$(objdir),$(<D))/$(@F)

%.o:	%.asm
	@echo Assembling $<...
	@$(AS) $(ASFLAGS) $< -o $(subst $(srcdir),$(objdir),$(<D))/$(@F)

else

%.o:	%.c
	@echo Compiling $<...
	@$(CC) $(CFLAGS) -Wno-unused -Wno-conversion -Wno-missing-prototypes -c $< -o $@

%.o:	%.asm
	@echo Assembling $<...
	@$(AS) $(ASFLAGS) $< -o $@

%.o:
	@echo Compiling $<...
	@$(CC) $(CXXFLAGS) -c $< -o $@

endif

#
#	Generate dependencies for C/C++ files
#

ifdef DEPEND

%.d:	%.c
	@echo Generating depend file for $<...
	@$(CC) -MM -MT "$(subst $(srcdir),$(objdir),$(<D))/$(*F).o $(subst $(srcdir),$(objdir),$(<D))/$(@F)" -x c++ $(CFLAGS) $< >$(subst $(srcdir),$(objdir),$(<D))/$(@F)

%.d:	%.cpp
	@echo Generating depend file for $<...
	@$(CXX) -MM -MT "$(subst $(srcdir),$(objdir),$(<D))/$(*F).o $(subst $(srcdir),$(objdir),$(<D))/$(@F)" -x c++ $(CXXFLAGS) $< >$(subst $(srcdir),$(objdir),$(<D))/$(@F)

endif

#
#	Phony targets
#

init:

ifdef	DEBUG
	@echo Making debug build...
else
	@echo Making normal build...
endif
	@echo
	@mkdir -p $(foreach dir, $(alldir),$(objdir)$(dir))
	@mkdir -p $(srcdir)dep/generated

cleandep:
	@echo Removing depend files from $(objdir)...
	@for dir in $(alldir); do rm -f $(objdir)$$dir/*.d; done

clean:
	@echo Removing all files from $(objdir)...
	@rm -f -r $(objdir)
	@rm -f -r $(ctv.h)

ifdef	PERL
	@echo Removing all files generated with perl scripts...
	@rm -f -r $(driverlist)
endif
	@echo Removing executable file...
	@rm -f bin/$(NAME)

#
#	Rule to force recompilation of any target that depends on it
#

FORCE:
