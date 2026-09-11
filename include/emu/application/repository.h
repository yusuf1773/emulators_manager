#ifndef EMU_MANAGER_INCLUDE_EMU_APPLICATION_REPOSITORY_H_
#define EMU_MANAGER_INCLUDE_EMU_APPLICATION_REPOSITORY_H_

#include <string>
#include <vector>

namespace emu {

template <typename T>
class Repository {
 public:
  virtual ~Repository() = default;
  virtual const std::vector<T>& GetAll() const = 0;
  virtual const T& GetByName(const std::string& name) const = 0;
  virtual void Add(const T& value, bool allow_replace = false) = 0;
  virtual void Replace(const std::string& old_name, const T& value) = 0;
  virtual void RemoveByName(const std::string& name) = 0;
};

}

#endif
