#
# Config
#

# Set Lib
$:.unshift File.dirname(__FILE__) + "/lib"

#
# Defaults
#

DEFAULT_LIB_NAME = "pp"
DEFAULT_RANGE = (0..64)

#
# Paths
#

# Set Base Dir
BASE_PATH = Dir.pwd

# Set Include Dir
INCLUDE_PATH = File.join(BASE_PATH, "include")

# Set Lib Dir
EXPORT_PATH = File.join(INCLUDE_PATH, DEFAULT_LIB_NAME)

#
# Requires
#

require "utils/reflect"

#
# Generators
#

generators = [
  "args/count",
  "args/count/mask",
  "args/count/sequence",

  "args/exclude",
  "args/get",
  "args/insert",
  "args/replace",
  "args/slice",

  "equalities/eq",
  "equalities/gt",
  "equalities/gte",
  "equalities/lt",
  "equalities/lte",
  "equalities/not_eq",

  "maths/dec"

].flat_map do |path|
  require path

  generator = Reflect.factory_from_path(path)

  generator.generator_range =   DEFAULT_RANGE
  generator.export_path =       EXPORT_PATH

  generator
end

#
# Debug
#

puts ""
puts "----- ----- ----- ----- ----- ----- -----"
puts "-----                               -----"
puts "-----    Pre Processor Generator    -----"
puts "-----                               -----"
puts "----- ----- ----- ----- ----- ----- -----"

puts ""

puts "> Debug"
puts ""
puts "  base_path:\n  #{BASE_PATH}"

puts ""
puts "  include_path:\n  #{INCLUDE_PATH}"

puts ""
puts "  export_path:\n  #{EXPORT_PATH}"

#
# Exec
#

puts ""
puts "> Generating..."

generators.each do |generator|
  puts ""
  puts "  #{generator}:"

  generator.build
  generator.export

  puts "  => generated #{generator.generated.length} lines"
end

puts ""
puts "> Complete"
