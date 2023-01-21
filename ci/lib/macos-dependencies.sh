QT_VERSION=5.15.2  # do not change in ../install-dependencies to avoid breaking the windows build

main() {
    setup_python_cmd
    install_qt
    install_cmake
    install_ecm
    install_qpropgen
}
