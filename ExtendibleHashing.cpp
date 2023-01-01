#include <bits/stdc++.h>
using namespace std;


class Bucket
{
public:
    vector<int> elements;
    int sizeOfBucket;
    int localDepth;
    int bucketNo;

    Bucket(int localDepth, int sizeOfBucket, int bucketNo)
    {
        this->localDepth = localDepth;
        this->sizeOfBucket = sizeOfBucket;
        this->bucketNo = bucketNo;
    }

    int getLocalDepth()
    {
        return localDepth;
    }

    int getBucketNo()
    {
        return bucketNo;
    }

    int getNumberOfElements()
    {
        return elements.size();
    }

    void insert(int key)
    {
        elements.push_back(key);
    }
};

class Directory
{
public:
    int sizeOfBucket;
    int globalDepth;
    vector<Bucket *> bucketList;

    Directory(int bucketSize)
    {
        sizeOfBucket = bucketSize;
        globalDepth = 0;
        bucketList.push_back(new Bucket(0, sizeOfBucket, 0));
    }

    void splitBucket(int bucketNo)
    {
        int bno = bucketList[bucketNo]->getBucketNo();
        Bucket *prevBucket = bucketList[bno];
        int splitImage = bno + (1 << (globalDepth - 1));
        vector<int> prevElements = bucketList[bno]->elements;
        Bucket *curBucket = new Bucket(prevBucket->getLocalDepth() + 1, sizeOfBucket, bno);
        Bucket *newBucket = new Bucket(prevBucket->getLocalDepth() + 1, sizeOfBucket, splitImage);
        bucketList[splitImage] = newBucket;
        bucketList[bno] = curBucket;

        for (int i = 0; i < sizeOfBucket; i++)
        {
            insertKey(prevElements[i]);
        }
        delete prevBucket;
    }

    int bucketNoFromKey(int key)
    {
        return bucketList[(key % (1 << globalDepth))]->getBucketNo();
    }

    void mergeBuckets(int bucket1, int bucket2)
    {
        if (bucketList[bucket1]->getBucketNo() == bucketList[bucket2]->getBucketNo())
            return;

        for (auto ele : bucketList[bucket2]->elements)
        {
            bucketList[bucket1]->elements.push_back(ele);
        }
        Bucket *bt = bucketList[bucket2];
        bucketList[bucket2] = bucketList[bucket1];
        bucketList[bucket1]->localDepth--;
        delete bt;
    }

    void displayBuckets()
    {
        cout << "\n\n"
             << "Global Depth: " << globalDepth << endl
             << endl;
        unordered_map<Bucket *, int> vis;
        for (int i = 0; i < (1 << globalDepth); i++)
        {
            cout << "Bucket: " << i << endl;
            if (vis[bucketList[i]])
            {
                cout << "Point to some earlier bucket ! !" << endl
                     << endl;
                continue;
            }
            vis[bucketList[i]] = 1;
            cout << "Local Depth: " << bucketList[i]->getLocalDepth() << " Number of elements: " << bucketList[i]->getNumberOfElements() << endl;
            for (auto ele : bucketList[i]->elements)
            {
                cout << ele << ' ';
            }
            cout << endl
                 << endl;
        }
        cout << endl;
    }

    void insertKey(int key)
    {
        if (searchKey(key, 0) != -1)
        {
            cout << "Element Already Exists ! !\n\n";
            return;
        }
        int bucketNo = bucketNoFromKey(key);
        Bucket *candidateBucket = bucketList[bucketNo];
        int numElements = candidateBucket->getNumberOfElements();
        int localDepth = candidateBucket->getLocalDepth();

        if (numElements < sizeOfBucket)
        {
            bucketList[bucketNo]->insert(key);
            return;
        }
        else
        {
            if (localDepth == globalDepth)
            {
                increaseDepth();
                splitBucket(bucketNo);
                insertKey(key);
                return;
            }
            else
            {
                splitBucket(bucketNo);
                insertKey(key);
                return;
            }
        }
    }

    void deleteKey(int key)
    {
        int bucketNo = bucketNoFromKey(key);
        if (searchKey(key, 1) != -1)
        {
            Bucket *bucket = bucketList[bucketNo];

            bucket->elements.erase(find(bucket->elements.begin(), bucket->elements.end(),key));
            if (globalDepth == 0)
                return;
            int rem = key % (1 << (globalDepth - 1));

            int bucket1 = rem;
            int bucket2 = rem + (1 << (globalDepth - 1));

            if (bucketList[bucket1]->getBucketNo() != bucketList[bucket2]->getBucketNo())
            {
                if (bucketList[bucket1]->getNumberOfElements() + bucketList[bucket2]->getNumberOfElements() <= sizeOfBucket)
                {
                    mergeBuckets(bucket1, bucket2);
                }
            }

            int flag = 1;
            for (int i = 0; i < (1 << globalDepth); i++)
            {
                if (bucketList[i]->getLocalDepth() == globalDepth)
                {
                    flag = 0;
                }
            }
            if (flag)
            {
                reduceDepth();
            }
        }
    }

    void reduceDepth()
    {
        for (int i = 0; i < (1 << (globalDepth - 1)); i++)
        {
            mergeBuckets(i, i + (1 << (globalDepth - 1)));
        }
        globalDepth--;
        bucketList.resize(1 << globalDepth);
    }

    void increaseDepth()
    {
        globalDepth++;
        vector<Bucket *> prev = bucketList;
        bucketList.insert(bucketList.end(), prev.begin(), prev.end());
    }

    int searchKey(int key, int flag)
    {
        int bucketNo = bucketNoFromKey(key);
        int find = 0;
        for (int i = 0; i < bucketList[bucketNo]->getNumberOfElements(); i++)
        {
            if (bucketList[bucketNo]->elements[i] == key)
            {
                find = 1;
                break;
            }
        }
        if (find)
        {
            if (flag)
            {
                cout << endl
                     << "Key found at bucket no: " << bucketList[bucketNo]->getBucketNo();
                cout << endl
                     << endl;
            }

            return bucketList[bucketNo]->getBucketNo();
        }
        else
        {
            if (flag)
            {
                cout << endl
                     << "Key not found ! !" << endl
                     << endl;
            }
            return -1;
        }
    }

    int numOfBuckets()
    {
        return bucketList.size();
    }
};

void initialMessage()
{
    cout << "\n\n";
    cout << "1 key : Insert Key\n";
    cout << "2 key : Delete Key\n";
    cout << "3 key : Search Key\n";
    cout << "4     : Display\n";
    cout << "5     : Exit\n\n";
}

int main()
{
    cout << "Extendible Hashing in C++\n\n";
    cout << "Enter Max Bucket Size : ";
    int bucketSize;
    cin >> bucketSize;
    Directory dir(bucketSize);
    initialMessage();

    while (1)
    {
        int flag;
        cin >> flag;

        if (flag == 1)
        {
            int key;
            cin >> key;
            dir.insertKey(key);
            continue;
        }

        if (flag == 2)
        {
            int key;
            cin >> key;
            dir.deleteKey(key);
            continue;
        }

        if (flag == 3)
        {
            int key;
            cin >> key;
            dir.searchKey(key, 1);
            continue;
        }

        if (flag == 4)
        {
            dir.displayBuckets();
            continue;
        }

        if (flag == 5)
        {
            cout << "\n\nExiting...\n";
            break;
        }

        cout << " Invalid input ! ! \n";
        initialMessage();
    }
    cout << endl;
    return 0;
}