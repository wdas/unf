import usd_notice_broker as u
from pxr import Usd, Tf

s = Usd.Stage.CreateInMemory()

def testPredicate(notice):
    print("predicate")
    print(notice)
    #print(dir(notice))
    return True

def _objectsChanged(self, notice, sender=None):
    print("OBJECTS CHANGED RECEIVED!")
objectsChangedNotice = Tf.Notice.Register(u.BrokerNotice.ObjectsChanged,
                                                        _objectsChanged,
                                                        s)

a = u.TestNoticeWrapper.Init(5)

cache = u.NoticeCache(u.BrokerNotice.TestNotice)

myBroker = u.NoticeBroker.Create(s)
myBroker.BeginTransaction(testPredicate)
print("BEGIN TRANSACTION")
print(myBroker.IsInTransaction())
s.DefinePrim("/root")
myBroker.Process(a)
print("END TRANSACTION")
myBroker.EndTransaction()
print(myBroker.IsInTransaction())

print("INIT B")
b = u.TestNoticeWrapper.Init(2)
b.Send()
b.Send()
print("GETALL")
print(cache.GetAll())
print("MERGEALL")
print(cache.MergeAll())
print("GETALL")
print(cache.GetAll())
print(cache.GetAll()[0].GetCount())

'''
myBroker = u.NoticeBroker.Create(s)
print(dir(u))
print(dir(myBroker))
myBroker.BeginTransaction(testPredicate)
print(myBroker.IsInTransaction())
s.DefinePrim("/root")
notice = u.TestNoticeWrapper.Init(1)
print(notice)
'''
'''
print(u.TestNotice(1))
#myBroker.Process(u.TestNotice(1))
myBroker.EndTransaction()
print(myBroker.IsInTransaction())
'''
