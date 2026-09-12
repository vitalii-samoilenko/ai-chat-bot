#ifndef ETELEMETRY_COMMON_HPP
#define ETELEMETRY_COMMON_HPP

#define _ET_NS _et

#define _ET_RAW_COMBINE(lhs, rhs) lhs ## rhs
#define _ET_RES_COMBINE(lhs, rhs) _ET_RAW_COMBINE(lhs, rhs)

#define _ET_PREPEND(identifier) _ET_RES_COMBINE(_ET_NS, identifier)

#endif
