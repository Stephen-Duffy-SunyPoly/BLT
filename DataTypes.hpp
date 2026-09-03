#pragma once

#include <memory>
#include <utility>

enum DataTypeValues {
    VOID_TYPE_VALUE = 0,
    INT_TYPE_VALUE = 1,
    FIXED_TYPE_VALUE = 2,
    POINTER_TYPE_VALUE = 3
};

class DataType {
public:
    virtual ~DataType() = default;

    /**Get the unique id of this data type
     * @return The unique id of this data type
     */
    virtual DataTypeValues getTypeId() = 0;

    /**Check if this type is exactly equal to other type
     * @param other The other data type to check
     * @return true if the data types are exactly the same
     */
    virtual bool typeEqual(DataType* other) = 0;

    /**Check if this data type is a pointer type
     * @return true if this data type is a pointer
     */
    virtual bool isPointer() {
        return false;
    }

    /**Get a pointer to the subtype of this data type
     * @return A pointer to the sub type of this data type or null if there is not sub type
     */
    virtual std::shared_ptr<DataType> getSubType() {
        return nullptr;
    }
};

class VoidType : public DataType {
    const DataTypeValues TYPE_ID = VOID_TYPE_VALUE;
public:
    DataTypeValues getTypeId() override {
        return TYPE_ID;
    }
    bool typeEqual(DataType* other) override {
        return TYPE_ID == other->getTypeId();
    }
};

class IntType : public DataType {
    const DataTypeValues TYPE_ID = INT_TYPE_VALUE;
public:
    DataTypeValues getTypeId() override {
        return TYPE_ID;
    }
    bool typeEqual(DataType* other) override {
        return TYPE_ID == other->getTypeId();
    }
};

class FixedType : public DataType {
    const DataTypeValues TYPE_ID = FIXED_TYPE_VALUE;
public:
    DataTypeValues getTypeId() override {
        return TYPE_ID;
    }
    bool typeEqual(DataType* other) override {
        return TYPE_ID == other->getTypeId();
    }
};

class PointerType : public DataType {
    const DataTypeValues TYPE_ID = POINTER_TYPE_VALUE;
    const std::shared_ptr<DataType> referenceType;
public:
    explicit PointerType(std::shared_ptr<DataType> referenceType) : referenceType(std::move(referenceType)) {}
    DataTypeValues getTypeId() override {
        return TYPE_ID;
    }
    bool typeEqual(DataType* other) override {
        return TYPE_ID == other->getTypeId() && referenceType->typeEqual(other->getSubType().get());
    }
    bool isPointer() override {
        return true;
    }
    std::shared_ptr<DataType> getSubType() override {
        return referenceType;
    }
};

/**Check if 2 data types are able to be combined in math operations. Note: the order does matter. Does not check pointer referenced types
 * @param left The first type to check
 * @param right The second type to check
 * @return true if the provided types can generally be matched together
 */
inline bool dataTypeCompatible(const DataTypeValues left, const DataTypeValues right) {
    //somthing is allways compatible with its self
    if (left == right) {
        return true;
    }
    if (left == VOID_TYPE_VALUE || right == VOID_TYPE_VALUE) {
        return false;
    }
    //fixed is not directly compatible with any other number type
    if (left == FIXED_TYPE_VALUE || right == FIXED_TYPE_VALUE) {
        return false;
    }
    if (left == POINTER_TYPE_VALUE && right == INT_TYPE_VALUE) {
        return true;
    }
    return false;
}

inline bool dataTypeCompatible(DataType* left, DataType* right) {
    return dataTypeCompatible(left->getTypeId(), right->getTypeId());
}