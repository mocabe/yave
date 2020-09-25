# boost
set(YAVE_BOOST_COMPONENTS random program_options filesystem)
find_package(Boost ${YAVE_BOOST_VERSION} COMPONENTS ${YAVE_BOOST_COMPONENTS} REQUIRED)