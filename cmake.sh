# Capstone disassembler engine (www.capstone-engine.org)
# Build Capstone libs for specified architecture, or all if none is specified (libcapstone.so & libcapstone.a) on *nix with CMake & make
# By Nguyen Anh Quynh, Jorn Vernee, 2019

FLAGS="-DCMAKE_BUILD_TYPE=Release"
# Uncomment below line to compile in Diet mode
# FLAGS+=" -DCAPSTONE_BUILD_DIET=ON"

case $1 in
  ARM)
    FLAGS+=" -DCAPSTONE_ARCHITECTURE_DEFAULT=OFF -DCAPSTONE_ARM_SUPPORT=ON"
    ;;
  ARM64)
    FLAGS+=" -DCAPSTONE_ARCHITECTURE_DEFAULT=OFF -DCAPSTONE_ARM64_SUPPORT=ON"
    ;;
  M68K)
    FLAGS+=" -DCAPSTONE_ARCHITECTURE_DEFAULT=OFF -DCAPSTONE_M68K_SUPPORT=ON"
    ;;
  MIPS)
    FLAGS+=" -DCAPSTONE_ARCHITECTURE_DEFAULT=OFF -DCAPSTONE_MIPS_SUPPORT=ON"
    ;;
  PowerPC)
    FLAGS+=" -DCAPSTONE_ARCHITECTURE_DEFAULT=OFF -DCAPSTONE_PPC_SUPPORT=ON"
    ;;
  Sparc)
    FLAGS+=" -DCAPSTONE_ARCHITECTURE_DEFAULT=OFF -DCAPSTONE_SPARC_SUPPORT=ON"
    ;;
  SystemZ)
    FLAGS+=" -DCAPSTONE_ARCHITECTURE_DEFAULT=OFF -DCAPSTONE_SYSZ_SUPPORT=ON"
    ;;
  XCore)
    FLAGS+=" -DCAPSTONE_ARCHITECTURE_DEFAULT=OFF -DCAPSTONE_XCORE_SUPPORT=ON"
    ;;
  x86)
    FLAGS+=" -DCAPSTONE_ARCHITECTURE_DEFAULT=OFF -DCAPSTONE_X86_SUPPORT=ON"
    ;;
  TMS320C64x)
    FLAGS+=" -DCAPSTONE_ARCHITECTURE_DEFAULT=OFF -DCAPSTONE_TMS320C64X_SUPPORT=ON"
    ;;
  M680x)
    FLAGS+=" -DCAPSTONE_ARCHITECTURE_DEFAULT=OFF -DCAPSTONE_M680X_SUPPORT=ON"
    ;;
  EVM)
    FLAGS+=" -DCAPSTONE_ARCHITECTURE_DEFAULT=OFF -DCAPSTONE_EVM_SUPPORT=ON"
    ;;
  MOS65XX)
    FLAGS+=" -DCAPSTONE_ARCHITECTURE_DEFAULT=OFF -DCAPSTONE_MOS65XX_SUPPORT=ON"
    ;;
  *)
    ;;
esac	
	
cmake $FLAGS ..

make -j8
