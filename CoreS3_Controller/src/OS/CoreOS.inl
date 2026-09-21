template <typename T>
void CoreOS::freePSRAM(T*& ptr) {
    if (ptr != nullptr) {
        heap_caps_free(static_cast<void*>(ptr));
        ptr = nullptr;
    }
}