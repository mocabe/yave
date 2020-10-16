# boost
set(YAVE_BOOST_COMPONENTS random program_options filesystem)
find_package(Boost COMPONENTS ${YAVE_BOOST_COMPONENTS} REQUIRED)