require 'ruby/extensions/string'

module ActiveRecord

  module Taggable
    module Base
      
      module RelationMethods
        extend ActiveSupport::Concern

        included do; end;

        module ClassMethods
          def taggable?
            false
          end

          def is_taggable
            is_taggable_on :tags
          end

          def is_taggable_on *args
            args = args.to_a.flatten.compact.map(&:to_sym)

            return unless args.length

            if taggable?
              self.tag_types = (self.tag_types + args).uniq # and return if taggable?
            else
              class_attribute :tag_types
              class_eval do
                has_many :taggables_relationships, as: :taggable, autosave: true, dependent: :destroy, class_name: 'ActiveRecord::Taggable::Models::TagRelationship'
                has_many :taggables, through: :taggables_relationships, source: :tag, class_name: 'ActiveRecord::Taggable::Models::Tag'

                def self.taggable?
                  true
                end
              end

              self.tag_types = args
            end

            taggable_accessors
          end

          protected

          def taggable_accessors
            include mixin

            tag_types.each do |tag_type|
              tag_type = tag_type.to_s.pluralize
              mixin.class_eval <<-RUBY, __FILE__, __LINE__ + 1
                def #{tag_type}
                  get_tags_on('#{tag_type}')
                end

                def #{tag_type}=(new_tags)
                  set_tags_on('#{tag_type}', new_tags)
                end
              RUBY
            end

          end

          def mixin 
            @mixin ||= Module.new
          end
        end

        def get_tags_on(context)
          taggables.where(["#{ActiveRecord::Taggable::Models::TagRelationship.table_name}.context = ?", context.to_s])
        end

        def set_tags_on(context, tags)
          tags = (case tags
            when String
              tags.split(',')
            else
              tags
            end
          )

          tags = tags.map do |tag|
            t = Taggable::Models::Tag.find_or_initialize_by(namespace: tag.to_slug)
            t.assign_attributes(name: tag) if t.new_record?
            t.save!

            t
          end

          tags_current = get_tags_on(context)

          tags_old = tags_current - tags
          tags_new = tags - tags_current

          tags_shared = tags_current & tags

          if tags_old.present?
            taggables_relationships.with_context(context).destroy_all(tag_id: tags_old)
          end

          tags_new.each do |tag|
            taggables_relationships.build tag: tag, context: context
          end
        end
      end

      ActiveRecord::Base.send(:include, RelationMethods)
    end
  end

end
