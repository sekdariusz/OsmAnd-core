#include "WorkerPool.h"
#include "WorkerPool_P.h"

OsmAnd::Concurrent::WorkerPool::WorkerPool(
    const Order order /*= Order::FIFO*/,
    const int maxThreadCount /*= QThread::idealThreadCount()*/)
    : _p(new WorkerPool_P(this, order, maxThreadCount))
{
}

OsmAnd::Concurrent::WorkerPool::~WorkerPool()
{
    reset();
}

OsmAnd::Concurrent::WorkerPool::Order OsmAnd::Concurrent::WorkerPool::order() const
{
    return _p->order();
}

void OsmAnd::Concurrent::WorkerPool::setOrder(const Order order)
{
    _p->setOrder(order);
}

int OsmAnd::Concurrent::WorkerPool::maxThreadCount() const
{
    return _p->maxThreadCount();
}

void OsmAnd::Concurrent::WorkerPool::setMaxThreadCount(int maxThreadCount)
{
    _p->setMaxThreadCount(maxThreadCount);
}

unsigned int OsmAnd::Concurrent::WorkerPool::activeThreadCount() const
{
    return _p->activeThreadCount();
}

bool OsmAnd::Concurrent::WorkerPool::waitForDone(const int msecs /*= -1*/) const
{
    return _p->waitForDone(msecs);
}

void OsmAnd::Concurrent::WorkerPool::enqueue(QRunnable* const runnable)
{
    _p->enqueue(runnable);
}

bool OsmAnd::Concurrent::WorkerPool::dequeue(QRunnable* const runnable)
{
    return _p->dequeue(runnable);
}

void OsmAnd::Concurrent::WorkerPool::dequeueAll()
{
    _p->dequeueAll();
}

void OsmAnd::Concurrent::WorkerPool::reset()
{
    _p->reset();
}
