#ifndef TLINK_H
#define TLINK_H

#include "Macros.h"

/** TLink: Encapsulates a node within a linked list. */
template<typename ElemType>
class TLink
{
    /** Pointer to the previous link in the list. */
    TLink* mpPrevLink = nullptr;

    /** Pointer to the next link in the list. */
    TLink* mpNextLink = nullptr;

    /** Data element contained in this node. */
    ElemType mElement{};

public:
    TLink() = default;

    TLink(const ElemType& rkInElement)
        : mElement(rkInElement)
    {}

    ~TLink()
    {
        Unlink();
    }

    /** Links this node into the provided list */
    void Link(TLink* pList)
    {
        if (mpPrevLink)
        {
            Unlink();
        }

        if (pList)
        {
            mpPrevLink = pList;
            mpNextLink = pList->mpNextLink;
            pList->mpNextLink = this;
        }

        pList = this;
    }

    /** Links this node in as the head of the provided list. The link passed in should be the head node of the list.
     *  The pointer will be updated to point to this node (the new head). */
    void LinkHead(TLink*& rpListHead)
    {
        mpPrevLink = rpListHead->mpPrevLink;
        mpNextLink = rpListHead;
        rpListHead->mpPrevLink = this;
        rpListHead = this;
    }

    /** Removes this node from the list it's linked into */
    void Unlink()
    {
        if (mpPrevLink)
        {
            mpPrevLink->mpNextLink = mpNextLink;
        }
        if (mpNextLink)
        {
            mpNextLink->mpPrevLink = mpPrevLink;
        }

        mpPrevLink = nullptr;
        mpNextLink = nullptr;
    }

    /** Assignment */
    TLink& operator=(const ElemType& rkElem)
    {
        mElement = rkElem;
        return *this;
    }

    /** Iteration */
    TLink* Previous() const         { return mpPrevLink; }
    TLink* Next() const             { return mpNextLink; }
    ElemType& Element()             { return mElement; }
    const ElemType& Element() const { return mElement; }

    /** Iterator class for linked lists */
    class TIterator
    {
        /** Current link being iterated on */
        TLink* mpCurLink;

    public:
        TIterator(TLink* pInList)
            : mpCurLink(pInList)
        {}

        /** Returns whether we are finished iterating */
        bool IsFinished() const
        {
            return mpCurLink == nullptr;
        }

        /** Advances to the next resource */
        void Next()
        {
            mpCurLink = mpCurLink->Next();
        }

        /** Increment operators */
        TIterator& operator++()
        {
            Next();
            return *this;
        }

        TIterator operator++(int)
        {
            TIterator Copy = *this;
            Next();
            return Copy;
        }

        /** Dereference operators */
        ElemType& operator*() const  { return mpCurLink->Element(); }
        ElemType* operator->() const { return &mpCurLink->Element(); }
        explicit operator bool() const        { return mpCurLink != nullptr; }
    };
};

#endif // TLINK_H
