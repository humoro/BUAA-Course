class OperationType {
    private boolean hasParameter = false;
    private boolean isReturnType = false;

    void setHasParameter() {
        this.hasParameter = true;
    }

    void setReturnType() {
        this.isReturnType = true;
    }

    boolean getHasParameter() {
        return this.hasParameter;
    }

    boolean getReturnType() {
        return this.isReturnType;
    }
}
