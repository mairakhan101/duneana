cmake_minimum_required(VERSION 3.6)
project(supernovastufforwhatever CXX)

include_directories($ENV{BOOST_INC})
include_directories($ENV{CANVAS_INC})
include_directories($ENV{CANVAS_ROOT_IO_INC})
include_directories($ENV{CETLIB_EXCEPT_INC})
include_directories($ENV{CETLIB_INC})
include_directories($ENV{FHICLCPP_INC})
include_directories($ENV{GALLERY_INC})
include_directories($ENV{LARCOREOBJ_INC})
include_directories($ENV{NUSIMDATA_INC})
include_directories($ENV{ROOT_INC})
include_directories($ENV{LARDATAOBJ_INC})
message($ENV{LARDATAOBJ_INC})
link_directories($ENV{ROOTSYS}/lib $ENV{CANVAS_LIB} $ENV{CETLIB_EXCEPT_LIB} $ENV{CETLIB_LIB} $ENV{GALLERY_LIB} $ENV{NUSIMDATA_LIB} $ENV{LARCOREOBJ_LIB} $ENV{LARDATAOBJ_INC})

add_executable(drawTruth drawTruth.C)

set_property(TARGET drawTruth PROPERTY CXX_STANDARD 26)

target_link_libraries(drawTruth Core RIO Net Hist Graf Graf3d Gpad Tree Rint Postscript Matrix Physics MathCore Thread MultiProc pthread canvas cetlib_except cetlib gallery nusimdata_SimulationBase larcoreobj_SummaryData)