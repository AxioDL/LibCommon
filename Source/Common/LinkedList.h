#ifndef TLINK_H
#define TLINK_H

#include "Macros.h"

/** TLink: Encapsulates a node within a linked list. */
template<typename ElemType>
class TLink
{
    /** Pointer to the previous link in the list. */
    TLink* mpPrevLink;

    /** Pointer to the next link in the list. */
    TLink* mpNextLink;

    /** Data element contained in this node. */
    ElemType mElement;

public:
    TLink()
        : mpPrevLink(nullptr)
        , mpNextLink(nullptr)
    {}

    TLink(const ElemType& rkInElement)
        : mpPrevLink(nullptr)
        , mpNextLink(nullptr)
        , mElement(rkInElement)
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
    inline TLink& operator=(const ElemType& rkElem)
    {
        mElement = rkElem;
        return *this;
    }

    /** Iteration */
    inline TLink* Previous() const         { return mpPrevLink; }
    inline TLink* Next() const             { return mpNextLink; }
    inline ElemType& Element()             { return mElement; }
    inline const ElemType& Element() const { return mElement; }

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
        inline TIterator& operator++()
        {
            Next();
            return *this;
        }

        inline TIterator& operator++(int)
        {
            TIterator Copy = *this;
            Next();
            return Copy;
        }

        /** Dereference operators */
        inline ElemType& operator*() const  { return mpCurLink->Element(); }
        inline ElemType* operator->() const { return &mpCurLink->Element(); }
        inline operator bool() const        { return mpCurLink != nullptr; }
    };
};

#endif // TLINK_H
