#!/bin/bash

init=1
tooldaq=1
boostflag=1
zmq=1
final=1
caenpp=1
rootflag=0
setup=0
threads=`nproc --all`

while [ ! $# -eq 0 ]
do
    case "$1" in
	--help | -h)
	    echo "This script should be run once after initially cloning the ToolApplication repository. It retrieves the ToolFrameworkCore TooDAQFramework ZMQ and BOOST repositories that provides the core framework and dependancies on which your application will be built."
	    exit
	    ;;

	--with_root | -r)
	    echo "Installing ToolDAQ with root"
	    rootflag=1 
	    ;;
	
	--no_boost | -b)
            echo "Installing ToolDAQ without boost"
            boostflag=0
	    ;;
	
	--no_init )
	     echo "Installing ToolDAQ without creating ToolDAQ Folder"
	    init=0;
	    ;;

	--no_zmq )
            echo "Installing ToolDAQ without zmq"
            zmq=0
            ;;

        --no_caenpp )
	    echo "Installing ToolDAQ without caen++"
	    caenpp=0
	    ;;

	--no_tooldaq )
	    echo "Installing dependancies without ToolDAQ"
	    tooldaq=0
	    ;;

	--no_final )
            echo "Installing ToolDAQ without compiling ToolAnalysis"
            final=0
            ;;

	--ToolDAQ_ZMQ )
            echo "Installing ToolDAQ & ZMQ"
	    boostflag=0
	    rootflag=0
	    final=0
            ;;

	--Boost )
            echo "Installing Boost"
	    init=0
	    tooldaq=0
	    zmq=0
	    final=0
	    rootflag=0
            ;;

	--Root )
            echo "Installing Root"
	    init=0
	    tooldaq=0
	    boostflag=0
	    zmq=0
	    final=0
	    rootflag=1
            ;;
	
	
	--Final )
            echo "Compiling ToolDAQ"
	    init=0
	    tooldaq=0
	    boostflag=0
	    rootflag=0
	    zmq=0
            ;;

    esac
    shift
done

# Installs CAEN libraries.
# Arguments:
#   1: path to install to. Installs libraries to $1/lib and header files to
#      $1/include
install_caen() {
  (
    shopt -s nullglob

    # Check if the libraries are already installed locally or to a place known to GCC
    declare -a libs=(CAENComm CAENVME) found=(${libs[@]})
    declare -i i n=0
    IFS=:
    for dir in \
      "$1/lib" \
      $LD_LIBRARY_PATH \
      `g++ -print-search-dirs | sed -n 's,^libraries: =,,; T; p'`
    do
      for ((i = 0; i < ${#libs[@]}; ++i)); do
        if [[ -e "$dir/lib${libs[$i]}.so" ]]; then
          if [[ "${found[$i]}" != 1 ]]; then
            found[i]=1
            n+=1
          fi
        fi
      done
      ((n == ${#libs[@]})) && break
    done
    IFS=
    ((n == ${#libs[@]})) && return

    tmp=`mktemp -dt 'caen.XXXXXX'` || return $?
    trap 'rm -r "$tmp"' exit

    # Check if the archives are already available
    declare -a archives
    n=0
    IFS=$'\n'
    for lib in ${libs[@]}; do
      archives=("$1"/$lib*.tgz)
      ((${#archives[@]})) && n+=1
    done
    IFS=
    if ((n == ${#libs[@]})); then
      dir="$1"
    # else try installing from the WCTE repository
    elif git clone ssh://git@github.com/WCTEDAQ/CaenDependancies "$tmp/caen"; then
      dir="$tmp/caen"
    else # ask the user
      echo
      echo
      echo 'Distribution of CAEN libraries is resitricted by the license. Please download the following libraries from the CAEN web site:'
      echo 'CAENComm   https://www.caen.it/products/caencomm-library/'
      echo 'CAENVMELib https://www.caen.it/products/caencomm-library/'
      echo "and put them to $tmp or enter a different directory below"
      echo 'Hit Enter to continue or Ctrl-D to skip the installation'
      read -ep "[$tmp] " dir || return 0
      [[ -z $dir ]] && dir=$tmp
    fi

    if ! [[ -d "$1/lib" ]]; then
      mkdir -p "$1/lib"
    fi &&
    if ! [[ -d "$1/include" ]]; then
      mkdir -p "$1/include"
    fi || return 1

    for lib in ${libs[@]}; do
      archive=`ls "$dir/$lib"* | sort -Vr | head -n1`
      distr=`tar -tf "$archive" | head -n1`
      version=${archive#$dir/$lib*-v}
      version=${version%.tgz}
      if ! [[ -e "$1/${archive##*/}" ]]; then
        cp "$archive" "$1" && chmod -x "$1/${archive##*/}"
      fi &&
      tar -C "$tmp" -xf "$archive" &&
      cp -v "$tmp/$distr/lib/x64/lib$lib.so.v$version" "$1/lib" &&
      ln -svf "lib$lib.so.v$version" "$1/lib/lib$lib.so" &&
      cp -v "$tmp/$distr/include"/* "$1/include" || return 1
    done
  )
}

if [ $init -eq 1 ]
then
    
    mkdir Dependencies
fi

cd Dependencies

if [ $tooldaq -eq 1 ]
then
git clone https://github.com/ToolFramework/ToolFrameworkCore.git

cd ToolFrameworkCore
make clean
make -j $threads

export LD_LIBRARY_PATH=`pwd`/lib:$LD_LIBRARY_PATH
cd ../

fi

if [ $zmq -eq 1 ]
then
    git clone https://github.com/ToolDAQ/zeromq-4.0.7.git
    
    cd zeromq-4.0.7
    
    ./configure --prefix=`pwd`
    make -j $threads
    make install
    
    export LD_LIBRARY_PATH=`pwd`/lib:$LD_LIBRARY_PATH
    
    cd ../
fi

if [ $boostflag -eq 1 ]
then
    
    git clone https://github.com/ToolDAQ/boost_1_66_0.git
     
    cd boost_1_66_0

    rm -rf INSTALL    
    mkdir install 
    
    ./bootstrap.sh --prefix=`pwd`/install/  > /dev/null 2>/dev/null
    ./b2 install iostreams -j $threads
    
    export LD_LIBRARY_PATH=`pwd`/install/lib:$LD_LIBRARY_PATH
    cd ../
fi


if [ $rootflag -eq 1 ]
then
    
    wget https://root.cern.ch/download/root_v5.34.34.source.tar.gz
    tar zxvf root_v5.34.34.source.tar.gz
    rm -rf root_v5.34.34.source.tar.gz
    cd root
    
    ./configure --enable-rpath
    make -j $threads
    make install
    
    source ./bin/thisroot.sh
    
    cd ../
    
fi

if [ $tooldaq -eq 1 ]
then
    git clone https://github.com/ToolDAQ/ToolDAQFramework.git
    
    cd ToolDAQFramework
    make clean
    make -j $threads
    export LD_LIBRARY_PATH=`pwd`/lib:$LD_LIBRARY_PATH
    cd ../
    
fi

if [ $caenpp -eq 1 ]
then
    [[ ! -d caen ]] && mkdir caen
    install_caen caen
    export LD_LIBRARY_PATH=`pwd`/caen/lib:$LD_LIBRARY_PATH

    git clone https://github.com/WCTEDAQ/caenpp.git caen/caen++
    cd caen/caen++
    ./configure --prefix="$PWD/.." --without-digitizer &&
    CPATH=../include make -j $threads &&
    make install

    cd ../..
fi

cd ../

if [ $final -eq 1 ]
then
    
    echo "current directory"
    echo `pwd`
if [ $setup -eq 1 ]
then   
    cp -r ./Dependencies/ToolFrameworkCore/DataModel/* ./DataModel
    cp -r ./Dependencies/ToolDAQFramework/DataModel/* ./DataModel
    cp -r ./Dependencies/ToolFrameworkCore/UserTools/* ./UserTools
    cp -r ./Dependencies/ToolDAQFramework/UserTools/template/* ./UserTools/template
    cp -r ./Dependencies/ToolDAQFramework/configfiles/* ./configfiles
    mkdir src
    cp -r ./Dependencies/ToolDAQFramework/src/main.cpp ./src/
    cp ./Dependencies/ToolDAQFramework/Application/* ./
    git add DataModel/
    git add UserTools/*
    git add configfiles/*
    git add ./Makefile
    git add ./CMakeLists.txt
    git add ./Setup.sh
    git add ./src/main.cpp
    rm -f ./GetToolFramework.sh
    sed -i 's/setup=0/setup=0/' ./GetToolDAQ.sh
fi   
    make clean
    make -j $threads
    
    export LD_LIBRARY_PATH=`pwd`/lib:$LD_LIBRARY_PATH
fi
