int compare_int_asc(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

int compare_int_desc(const void* a, const void* b) {
    return (*(int*)b - *(int*)a);
}

int compare_float_asc(const void* a, const void* b) {
    float fa = *(float*)a, fb = *(float*)b;
    return (fa > fb) - (fa < fb);
}

int compare_float_desc(const void* a, const void* b) {
    float fa = *(float*)a, fb = *(float*)b;
    return (fb > fa) - (fb < fa);
}

int compare_double_asc(const void* a, const void* b) {
    double da = *(double*)a, db = *(double*)b;
    return (da > db) - (da < db);
}

int compare_double_desc(const void* a, const void* b) {
    double da = *(double*)a, db = *(double*)b;
    return (db > da) - (db < da);
}