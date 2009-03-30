    function basicSerialize (o)
      if type(o) == "number" then
        return tostring(o)
      else   -- assume it is a string
        return tostring(o);
--string.format("%q", o)
      end
    end


    function save (name, value, saved)
      saved = saved or {}       -- initial value
      io.write(name, " = ")
      if type(value) == "number" or type(value) == "string" then
        io.write(basicSerialize(value), "\n")
      elseif type(value) == "table" then
        if saved[value] then    -- value already saved?
          io.write(saved[value], "\n")  -- use its previous name
        else
          saved[value] = name   -- save name for next time
          io.write("{}\n")     -- create a new table
          for k,v in pairs(value) do      -- save its fields
            local fieldname = string.format("%s[%s]", name,
                                            basicSerialize(k))
            save(fieldname, v, saved)
          end
        end
      else
        error("cannot save a " .. type(value))
      end
    end

require 'zet'

idx = zet.index_load("zetdata/index", 10000000)

res = zet.index_search(idx, arg[1], 0, 100)

print("Total results: " .. tostring(res.total))

for i, r in ipairs(res) do
  print("Result " .. tostring(i) .. " : " .. tostring(r.score) .. ":" .. r.auxilliary )
  print(r.summary)
end

-- save("res", res)
